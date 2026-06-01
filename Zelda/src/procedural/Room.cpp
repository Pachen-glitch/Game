#include "Room.h"
#include "RoomTemplates.h"
#include "../core/Constants.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <utility>
#include <vector>

namespace {

void carveTiles(Map& map, DoorSide side, TileType tile) {
    const int midX = map.getWidth() / 2;
    const int midY = map.getHeight() / 2;
    const int w = map.getWidth();
    const int h = map.getHeight();

    switch (side) {
        case DoorSide::North:
            for (int dx = -1; dx <= 1; ++dx) {
                map.setTile(midX + dx, 0, tile);
            }
            break;
        case DoorSide::South:
            for (int dx = -1; dx <= 1; ++dx) {
                map.setTile(midX + dx, h - 1, tile);
            }
            break;
        case DoorSide::East:
            for (int dy = -1; dy <= 1; ++dy) {
                map.setTile(w - 1, midY + dy, tile);
            }
            break;
        case DoorSide::West:
            for (int dy = -1; dy <= 1; ++dy) {
                map.setTile(0, midY + dy, tile);
            }
            break;
        default:
            break;
    }
}

DoorSide oppositeSide(DoorSide side) {
    switch (side) {
        case DoorSide::North: return DoorSide::South;
        case DoorSide::South: return DoorSide::North;
        case DoorSide::East: return DoorSide::West;
        case DoorSide::West: return DoorSide::East;
        default: return DoorSide::None;
    }
}

sf::Vector2i openingCenterTile(const Map& map, DoorSide side) {
    const int midX = map.getWidth() / 2;
    const int midY = map.getHeight() / 2;

    switch (side) {
        case DoorSide::North: return {midX, 0};
        case DoorSide::South: return {midX, map.getHeight() - 1};
        case DoorSide::East: return {map.getWidth() - 1, midY};
        case DoorSide::West: return {0, midY};
        default: return {midX, midY};
    }
}

sf::Vector2i inwardDelta(DoorSide openingSide) {
    switch (openingSide) {
        case DoorSide::North: return {0, 1};
        case DoorSide::South: return {0, -1};
        case DoorSide::East: return {-1, 0};
        case DoorSide::West: return {1, 0};
        default: return {0, 0};
    }
}

bool isValidPlayerSpawn(const Map& map, sf::Vector2f worldPos) {
    const float size = static_cast<float>(Constants::TILE_SIZE);
    const sf::FloatRect box(worldPos.x, worldPos.y, size, size);

    auto cornerWalkable = [&](float x, float y) {
        sf::Vector2i tile = map.worldToTile({x, y});
        return map.isWalkable(tile.x, tile.y);
    };

    return cornerWalkable(box.left, box.top)
        && cornerWalkable(box.left + box.width - 1.f, box.top)
        && cornerWalkable(box.left, box.top + box.height - 1.f)
        && cornerWalkable(box.left + box.width - 1.f, box.top + box.height - 1.f);
}

sf::Vector2f trySpawnTile(const Map& map, int tx, int ty) {
    if (tx < 0 || ty < 0 ||
        tx >= map.getWidth() || ty >= map.getHeight()) {
        return {-1.f, -1.f};
    }

    sf::Vector2f pos = map.tileToWorld(tx, ty);
    return isValidPlayerSpawn(map, pos) ? pos : sf::Vector2f{-1.f, -1.f};
}

sf::Vector2f findOpeningAwareSpawn(const Map& map, DoorSide enteredFrom) {
    const DoorSide openingSide = oppositeSide(enteredFrom);
    const sf::Vector2i center = openingCenterTile(map, openingSide);
    const sf::Vector2i inward = inwardDelta(openingSide);

    const bool horizontalOpening =
        openingSide == DoorSide::North ||
        openingSide == DoorSide::South;

    for (int depth = 1; depth <= 8; ++depth) {
        const int bx = center.x + inward.x * depth;
        const int by = center.y + inward.y * depth;

        if (sf::Vector2f pos = trySpawnTile(map, bx, by); pos.x >= 0.f) {
            return pos;
        }

        for (int offset = 1; offset <= 3; ++offset) {
            if (horizontalOpening) {
                if (sf::Vector2f pos = trySpawnTile(map, bx + offset, by); pos.x >= 0.f) {
                    return pos;
                }
                if (sf::Vector2f pos = trySpawnTile(map, bx - offset, by); pos.x >= 0.f) {
                    return pos;
                }
            } else {
                if (sf::Vector2f pos = trySpawnTile(map, bx, by + offset); pos.x >= 0.f) {
                    return pos;
                }
                if (sf::Vector2f pos = trySpawnTile(map, bx, by - offset); pos.x >= 0.f) {
                    return pos;
                }
            }
        }
    }

    const int midX = map.getWidth() / 2;
    const int midY = map.getHeight() / 2;

    for (int radius = 0; radius <= 10; ++radius) {
        for (int dy = -radius; dy <= radius; ++dy) {
            for (int dx = -radius; dx <= radius; ++dx) {
                if (std::max(std::abs(dx), std::abs(dy)) != radius) continue;
                if (sf::Vector2f pos = trySpawnTile(map, midX + dx, midY + dy); pos.x >= 0.f) {
                    return pos;
                }
            }
        }
    }

    return map.tileToWorld(midX, midY);
}

} // namespace

void Room::buildLayout(int layoutSalt) {
    map = Map();

    uint8_t required = requiredOpeningMask(connections);
    const RoomTemplateDef* tmpl = pickRoomTemplate(type, required, layoutSalt);
    templateId = tmpl->id;
    loadTemplate(*tmpl->layout);
    applyConnectionTiles(false);
}

void Room::applyConnectionTiles(bool bossGateUnlocked) {
    for (const auto& conn : connections) {
        if (conn.isBossGate) {
            if (bossGateUnlocked) {
                carveEdge(conn.side, TileType::OPENING);
            } else {
                carveEdge(conn.side, TileType::LOCKED_DOOR);
            }
        } else {
            carveEdge(conn.side, TileType::OPENING);
        }
    }
}

void Room::carveEdge(DoorSide side, TileType tile) {
    carveTiles(map, side, tile);
}

bool Room::exitsAreOpen() const {
    return cleared ||
           type == RoomType::Start ||
           type == RoomType::Shop;
}

sf::Vector2f Room::getPlayerSpawn() const {
    float cx = (map.getWidth() / 2.f) * Constants::TILE_SIZE;
    float cy = (map.getHeight() / 2.f) * Constants::TILE_SIZE;
    return {cx, cy};
}

sf::Vector2f Room::getOpeningWorldPos(DoorSide side) const {
    return getOpeningBounds(side).getPosition();
}

sf::FloatRect Room::getOpeningBounds(DoorSide side) const {
    float midX = static_cast<float>(map.getWidth() / 2);
    float midY = static_cast<float>(map.getHeight() / 2);
    float ts = static_cast<float>(Constants::TILE_SIZE);
    const float w = ts * 3.f;
    const float h = ts;

    switch (side) {
        case DoorSide::North:
            return { (midX - 1.5f) * ts, 0.f, w, h };
        case DoorSide::South:
            return { (midX - 1.5f) * ts, (map.getHeight() - 1) * ts, w, h };
        case DoorSide::East:
            return { (map.getWidth() - 1) * ts, (midY - 1.5f) * ts, h, w };
        case DoorSide::West:
            return { 0.f, (midY - 1.5f) * ts, h, w };
        default:
            return { midX * ts, midY * ts, ts, ts };
    }
}

sf::Vector2f Room::getTransitionSpawn(DoorSide enteredFrom) const {
    return findOpeningAwareSpawn(map, enteredFrom);
}

TileType Room::charToTile(char c) {
    switch (c) {
        case '#': return TileType::WALL;
        case '.': return TileType::FLOOR;
        case 'D': return TileType::DOOR;
        case 'L': return TileType::LOCKED_DOOR;
        case 'T': return TileType::TREE;
        case 'W': return TileType::WATER;
        case 'R': return TileType::ROCK;
        case 'B': return TileType::BUSH;
        case 'P': return TileType::PIT;
        case 'S': return TileType::STATUE;
        default: return TileType::FLOOR;
    }
}

void Room::loadTemplate(const std::vector<std::string>& layout) {
    for (int y = 0; y < static_cast<int>(layout.size()); ++y) {
        for (int x = 0; x < static_cast<int>(layout[y].size()); ++x) {
            map.setTile(x, y, charToTile(layout[y][x]));
        }
    }
}
