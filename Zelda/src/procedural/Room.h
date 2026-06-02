#pragma once
#include "RoomType.h"
#include "../map/Map.h"
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <vector>
#include <string>

struct RoomConnection {
    int targetRoomId = -1;
    DoorSide side = DoorSide::None;
    bool locked = false;
    bool isBossGate = false;
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
    std::string templateId;

    void buildLayout(int layoutSalt);
    void applyConnectionTiles(bool bossGateUnlocked);

    sf::Vector2f getPlayerSpawn() const;
    sf::Vector2f getOpeningWorldPos(DoorSide side) const;
    sf::FloatRect getOpeningBounds(DoorSide side) const;
    sf::Vector2f getTransitionSpawn(DoorSide enteredFrom) const;

    sf::Vector2f getBossGateWorldPos() const;

    DoorSide getAntechamberEntrySide() const { return antechamberEntrySide; }
    DoorSide getAntechamberBossPassageSide() const {
        return antechamberBossPassageSide;
    }

    bool exitsAreOpen() const;

private:
    DoorSide antechamberEntrySide = DoorSide::None;
    DoorSide antechamberBossPassageSide = DoorSide::None;

    TileType charToTile(char c);
    void loadTemplate(const std::vector<std::string>& layout);
    void carveEdge(DoorSide side, TileType tile);
    void resolveAntechamberSides();
    void applyAntechamberConnectionTiles(bool bossGateUnlocked);
};
