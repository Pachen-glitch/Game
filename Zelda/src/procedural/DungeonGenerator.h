#pragma once

#include "Room.h"

#include <vector>

// Procedural Zelda-like room graph — flow from start → boss, weighted types.
class DungeonGenerator {
public:
    std::vector<Room> generate(int seed = 0);

private:
    RoomType pickWeightedRoom(bool needShop, bool needTreasure, int depth);
    void connectRooms(std::vector<Room>& rooms);
    int seed = 0;
};
