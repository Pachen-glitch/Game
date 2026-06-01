#include "Room.h"
#include "RoomTemplates.h"
#include "../core/Constants.h"

#include <algorithm>
#include <cstdlib>

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
    float ts = static_cast<float>(Constants::TILE_SIZE);

    switch (enteredFrom) {
        case DoorSide::North:
            return {
                (map.getWidth() / 2.f) * ts,
                (map.getHeight() - 3.f) * ts
            };
        case DoorSide::South:
            return {
                (map.getWidth() / 2.f) * ts,
                2.f * ts
            };
        case DoorSide::East:
            return {
                2.f * ts,
                (map.getHeight() / 2.f) * ts
            };
        case DoorSide::West:
            return {
                (map.getWidth() - 3.f) * ts,
                (map.getHeight() / 2.f) * ts
            };
        default:
            return getPlayerSpawn();
    }
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
