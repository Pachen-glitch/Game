#pragma once

#include "DungeonGenerator.h"
#include "../map/Map.h"
#include "../entity/base/EntityManager.h"
#include "../entity/player/Player.h"

// Active run state — current room, dungeon graph, transitions.
class World {
public:
    void newRun(int seed = 0);
    void loadRoom(int roomId);
    void populateRoom();

    Room& currentRoom();
    const Room& currentRoom() const;
    int getCurrentRoomId() const { return currentRoomId; }
    const std::vector<Room>& getRooms() const { return rooms; }

    EntityManager& getEntities() { return entities; }
    void updateEnemies(Player& player, float dt, const Map& map);

    bool hasActiveNarutoBoss() const;
    bool debugSpawnNarutoNear(sf::Vector2f pos);
    bool debugRemoveNaruto();

    bool tryTransition(Player& player, DoorSide& outSide);

private:
    std::vector<Room> rooms;
    int currentRoomId = 0;
    EntityManager entities;
    DungeonGenerator generator;
};
