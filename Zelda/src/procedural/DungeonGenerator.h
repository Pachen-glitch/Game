#pragma once

#include "Room.h"

#include <vector>

// Procedural Zelda-like room graph — grid adjacency matches minimap.
class DungeonGenerator {
public:
    std::vector<Room> generate(int seed = 0, int floorIndex = 1, bool placeBoss = true);

private:
    RoomType pickWeightedRoom(bool needShop, bool needTreasure, int depth);
    void connectRooms(std::vector<Room>& rooms);
    int assignBossRoom(std::vector<Room>& rooms) const;
    int pickBossAntechamberId(const std::vector<Room>& rooms, int bossRoomId) const;
    void markBossGates(
        std::vector<Room>& rooms,
        int bossRoomId,
        int antechamberId
    );
    void validateBossGeneration(
        std::vector<Room>& rooms,
        int bossRoomId,
        int antechamberId
    );
    void buildAllLayouts(std::vector<Room>& rooms);

    int seed = 0;
};
