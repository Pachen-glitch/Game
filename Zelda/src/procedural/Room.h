#pragma once
#include "RoomType.h"
#include "../map/Map.h"
#include <SFML/System/Vector2.hpp>
#include <vector>
#include <string>

struct RoomConnection {
    int targetRoomId = -1;
    DoorSide side = DoorSide::None;
    bool locked = false;
};

// One dungeon cell — tile map + metadata + spawn lists.
class Room {
public:
    int id = 0;
    RoomType type = RoomType::Combat;
    Map map;
    std::vector<RoomConnection> connections;
    bool visited = false;
    bool cleared = false;

    sf::Vector2i gridPos{0, 0};

    void generateLayout();
    sf::Vector2f getPlayerSpawn() const;
    sf::Vector2f getDoorWorldPos(DoorSide side) const;
    sf::Vector2f getTransitionSpawn(DoorSide enteredFrom) const;
private:

    TileType charToTile(char c);

    void loadTemplate(
        const std::vector<std::string>& layout
    );
};


