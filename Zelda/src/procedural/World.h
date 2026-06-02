#pragma once

#include "DungeonGenerator.h"
#include "../map/Map.h"
#include "../entity/base/EntityManager.h"
#include "../entity/player/Player.h"

class Door;

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

    int getCurrentFloor() const { return currentFloor; }
    int getMaxFloors() const { return maxFloors; }
    bool isBossGateUnlocked() const { return bossGateUnlocked; }
    int getBossRoomId() const { return bossRoomId; }
    bool isInVictoryRoom() const { return inVictoryRoom; }

    EntityManager& getEntities() { return entities; }
    void updateEnemies(Player& player, float dt, const Map& map);
    void trySpawnVictoryDoorAfterBossDefeat();
    bool tryEnterVictoryRoom(Player& player);

    bool hasActiveNarutoBoss() const;
    bool debugSpawnNarutoNear(sf::Vector2f pos);
    bool debugRemoveNaruto();
    int debugKillNormalEnemies();

    bool tryTransition(Player& player, DoorSide& outSide);
    bool enterBossArena(Player& player);
    void unlockBossGate();

private:
    void generateCurrentFloor(int seed);
    void spawnConnectionDoors(Room& room);
    void spawnVictoryDoor();
    void buildVictoryRoom();
    bool canTraverseConnection(const Room& room, const RoomConnection& conn) const;
    bool playerAtOpening(const Player& player, const Room& room, DoorSide side) const;

    std::vector<Room> rooms;
    Room victoryRoom;
    int currentRoomId = 0;
    int bossRoomId = -1;
    int currentFloor = 1;
    int maxFloors = 1;
    bool bossGateUnlocked = false;
    bool inVictoryRoom = false;
    bool victoryDoorSpawned = false;
    bool victoryRoomBuilt = false;
    EntityManager entities;
    DungeonGenerator generator;
    int runSeed = 0;
    float transitionCooldown = 0.f;
};
