#include "Room.h"
#include "../core/Constants.h"

#include <cstdlib>

void Room::generateLayout() {
    map = Map();

    static const std::vector<std::string> combatA = {

        "####################",
        "#..................#",
        "#....T......T......#",
        "#..................#",
        "#........WW........#",
        "#..................#",
        "#....R......R......#",
        "#..................#",
        "#..................#",
        "#..................#",
        "#....S......S......#",
        "#..................#",
        "#........PP........#",
        "#..................#",
        "#....B......B......#",
        "#..................#",
        "#..................#",
        "#..................#",
        "#..................#",
        "####################"
    };

    static const std::vector<std::string> shopA = {

        "####################",
        "#..................#",
        "#..................#",
        "#..................#",
        "#..................#",
        "#..................#",
        "#......SSSS........#",
        "#......S..S........#",
        "#......S..S........#",
        "#......SSSS........#",
        "#..................#",
        "#..................#",
        "#..................#",
        "#..................#",
        "#..................#",
        "#..................#",
        "#..................#",
        "#..................#",
        "#..................#",
        "####################"
    };

    static const std::vector<std::string> shopB = {

        "####################",
        "#..................#",
        "#..................#",
        "#..................#",
        "#....SS......SS....#",
        "#....S........S....#",
        "#....S........S....#",
        "#....SS......SS....#",
        "#..................#",
        "#..................#",
        "#..................#",
        "#..................#",
        "#..................#",
        "#..................#",
        "#..................#",
        "#..................#",
        "#..................#",
        "#..................#",
        "#..................#",
        "####################"
    };

    static const std::vector<std::string> treasureB = {

        "####################",
        "#..................#",
        "#..................#",
        "#....WWWWWWWW......#",
        "#....W......W......#",
        "#....W......W......#",
        "#....W......W......#",
        "#....WWWWWWWW......#",
        "#..................#",
        "#..................#",
        "#......SSSS........#",
        "#......S..S........#",
        "#......S..S........#",
        "#......SSSS........#",
        "#..................#",
        "#..................#",
        "#..................#",
        "#..................#",
        "#..................#",
        "####################"
    };

    static const std::vector<std::string> combatB = {
        "####################",
        "#..................#",
        "#....TT....TT......#",
        "#..................#",
        "#......RRRR........#",
        "#..................#",
        "#....TT....TT......#",
        "#..................#",
        "#..................#",
        "#..................#",
        "#......WWWW........#",
        "#..................#",
        "#..................#",
        "#..................#",
        "#....BB....BB......#",
        "#..................#",
        "#..................#",
        "#..................#",
        "#..................#",
        "####################"
    };

    static const std::vector<std::string> combatC = {
        "####################",
        "#..................#",
        "#..RR........RR....#",
        "#..................#",
        "#..................#",
        "#....WW....WW......#",
        "#..................#",
        "#..................#",
        "#..RR........RR....#",
        "#..................#",
        "#..................#",
        "#....TT....TT......#",
        "#..................#",
        "#..................#",
        "#..................#",
        "#....BB....BB......#",
        "#..................#",
        "#..................#",
        "#..................#",
        "####################"
    };

    static const std::vector<std::string> treasureA = {

        "####################",
        "#..................#",
        "#..................#",
        "#..................#",
        "#......SSSS........#",
        "#......S..S........#",
        "#......S..S........#",
        "#......SSSS........#",
        "#..................#",
        "#..................#",
        "#........WW........#",
        "#..................#",
        "#..................#",
        "#..................#",
        "#..................#",
        "#..................#",
        "#..................#",
        "#..................#",
        "#..................#",
        "####################"
    };
    if (type == RoomType::Combat || type == RoomType::Boss) {

        std::vector<const std::vector<std::string>*> combatLayouts = {

            &combatA,
            &combatB,
            &combatC
        };

        int layoutIndex = std::rand() % combatLayouts.size();

        loadTemplate(*combatLayouts[layoutIndex]);
    }
    else if (type == RoomType::Treasure) {

        std::vector<const std::vector<std::string>*> treasureLayouts = {

            &treasureA,
            &treasureB
        };

        int layoutIndex = std::rand() % treasureLayouts.size();

        loadTemplate(*treasureLayouts[layoutIndex]);
    }
    else if (type == RoomType::Shop) {

        loadTemplate(shopA);
    }
    else {

        loadTemplate(combatA);
    }
    for (const auto& conn : connections) {
        int midX = map.getWidth() / 2;
        int midY = map.getHeight() / 2;

        switch (conn.side) {

            case DoorSide::North:
                map.setTile(
                    midX,
                    0,
                    conn.locked ? TileType::LOCKED_DOOR
                                : TileType::DOOR
                );
                break;

            case DoorSide::South:
                map.setTile(
                    midX,
                    map.getHeight() - 1,
                    conn.locked ? TileType::LOCKED_DOOR
                                : TileType::DOOR
                );
                break;

            case DoorSide::East:
                map.setTile(
                    map.getWidth() - 1,
                    midY,
                    conn.locked ? TileType::LOCKED_DOOR
                                : TileType::DOOR
                );
                break;

            case DoorSide::West:
                map.setTile(
                    0,
                    midY,
                    conn.locked ? TileType::LOCKED_DOOR
                                : TileType::DOOR
                );
                break;

            default:
                break;
        }
    }
}

sf::Vector2f Room::getPlayerSpawn() const {
    float cx = (map.getWidth() / 2.f) * Constants::TILE_SIZE;
    float cy = (map.getHeight() / 2.f) * Constants::TILE_SIZE;
    return {cx, cy};
}

sf::Vector2f Room::getDoorWorldPos(DoorSide side) const {
    float midX = static_cast<float>(map.getWidth() / 2);
    float midY = static_cast<float>(map.getHeight() / 2);
    float ts = static_cast<float>(Constants::TILE_SIZE);

    switch (side) {

        case DoorSide::North:
            return {midX * ts, ts};

        case DoorSide::South:
            return {midX * ts, (map.getHeight() - 2) * ts};

        case DoorSide::East:
            return {(map.getWidth() - 2) * ts, midY * ts};

        case DoorSide::West:
            return {ts, midY * ts};

        default:
            return {midX * ts, midY * ts};
    }
}

sf::Vector2f Room::getTransitionSpawn(DoorSide enteredFrom) const
{
    float ts = static_cast<float>(Constants::TILE_SIZE);

    switch (enteredFrom)
    {
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
            break;
    }
}


TileType Room::charToTile(char c) {

    switch (c) {

        case '#':
            return TileType::WALL;

        case '.':
            return TileType::FLOOR;

        case 'D':
            return TileType::DOOR;

        case 'L':
            return TileType::LOCKED_DOOR;

        case 'T':
            return TileType::TREE;

        case 'W':
            return TileType::WATER;

        case 'R':
            return TileType::ROCK;

        case 'B':
            return TileType::BUSH;

        case 'P':
            return TileType::PIT;

        case 'S':
            return TileType::STATUE;

        default:
            return TileType::FLOOR;
    }
}

void Room::loadTemplate(
    const std::vector<std::string>& layout
) {
    for (int y = 0; y < static_cast<int>(layout.size()); ++y) {
        for (int x = 0; x < static_cast<int>(layout[y].size()); ++x) {

            TileType tile = charToTile(layout[y][x]);

            map.setTile(x, y, tile);
        }
    }
}