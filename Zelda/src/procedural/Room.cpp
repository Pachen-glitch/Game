#include "Room.h"
#include "../core/Constants.h"

#include <cstdlib>

void Room::generateLayout() {
    map = Map();

    // Open arena-style rooms — perimeter walls, open floor (not maze).
    for (int y = 0; y < map.getHeight(); ++y) {
        for (int x = 0; x < map.getWidth(); ++x) {
            bool border = (x == 0 || y == 0 ||
                x == map.getWidth() - 1 || y == map.getHeight() - 1);
            map.setTile(x, y, border ? TileType::WALL : TileType::FLOOR);
        }
    }

    // Door openings on sides that connect
    for (const auto& conn : connections) {
        int midX = map.getWidth() / 2;
        int midY = map.getHeight() / 2;
        switch (conn.side) {
            case DoorSide::North: map.setTile(midX, 0, TileType::FLOOR); break;
            case DoorSide::South: map.setTile(midX, map.getHeight() - 1, TileType::FLOOR); break;
            case DoorSide::East:  map.setTile(map.getWidth() - 1, midY, TileType::FLOOR); break;
            case DoorSide::West:  map.setTile(0, midY, TileType::FLOOR); break;
            default: break;
        }
    }

    // Light pillars for combat rooms — not labyrinth pillars
    if (type == RoomType::Combat || type == RoomType::Boss) {
        for (int i = 0; i < 4; ++i) {
            int px = 4 + (i % 2) * 10;
            int py = 4 + (i / 2) * 10;
            map.setTile(px, py, TileType::WALL);
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
            return getPlayerSpawn();
    }
}
