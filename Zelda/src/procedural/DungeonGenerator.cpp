#include "DungeonGenerator.h"
#include "../core/Constants.h"

#include <SFML/System/Vector2.hpp>

#include <cstdlib>
#include <ctime>
#include <queue>
#include <set>

static int rnd(int seed, int mod) {
    return (std::abs(seed * 1103515245 + 12345) % mod);
}

RoomType DungeonGenerator::pickWeightedRoom(
    bool needShop, bool needTreasure, int depth
) {
    int roll = rnd(seed + depth, 100);
    if (needShop && roll < 25) return RoomType::Shop;
    if (needTreasure && roll < 40) return RoomType::Treasure;
    if (roll < 55) return RoomType::Combat;
    if (roll < 75) return RoomType::Corridor;
    return RoomType::Combat;
}

std::vector<Room> DungeonGenerator::generate(int s) {
    seed = s == 0 ? static_cast<int>(std::time(nullptr)) : s;
    std::srand(seed);

    int roomCount = Constants::MIN_ROOMS +
        rnd(seed, Constants::MAX_ROOMS - Constants::MIN_ROOMS + 1);

    std::vector<Room> rooms;
    rooms.reserve(roomCount);

    std::set<std::pair<int,int>> used;
    std::queue<std::pair<int,int>> frontier;
    frontier.push({0, 0});
    used.insert({0, 0});

    bool shopPlaced = false;
    bool treasurePlaced = false;

    for (int i = 0; i < roomCount; ++i) {
        if (frontier.empty()) {
            // La expansion aleatoria puede vaciar la cola antes de crear todas las salas.
            int nx = rooms.empty() ? 0 : rooms.back().gridPos.x + 1;
            int ny = rooms.empty() ? 0 : rooms.back().gridPos.y;
            while (used.count({nx, ny})) {
                nx++;
                if (nx > 32) { nx = 0; ny++; }
            }
            used.insert({nx, ny});
            frontier.push({nx, ny});
        }

        Room room;
        room.id = i;
        auto gp = frontier.front();
        room.gridPos = sf::Vector2i(gp.first, gp.second);
        frontier.pop();

        if (i == 0) {
            room.type = RoomType::Start;
        } else if (i == roomCount - 1) {
            room.type = RoomType::Boss;
        } else {
            room.type = pickWeightedRoom(!shopPlaced, !treasurePlaced, i);
            if (room.type == RoomType::Shop) shopPlaced = true;
            if (room.type == RoomType::Treasure) treasurePlaced = true;
        }

        room.generateLayout();
        rooms.push_back(room);

        // expand graph in cardinal directions
        const int dirs[4][2] = {{0,-1},{0,1},{1,0},{-1,0}};
        for (int d = 0; d < 4; ++d) {
            int nx = room.gridPos.x + dirs[d][0];
            int ny = room.gridPos.y + dirs[d][1];
            if (used.count({nx, ny}) == 0 && rnd(seed + i + d, 100) < 45) {
                used.insert({nx, ny});
                frontier.push({nx, ny});
            }
        }
    }

    connectRooms(rooms);
    return rooms;
}

void DungeonGenerator::connectRooms(std::vector<Room>& rooms) {
    // Linear flow with occasional branches — Isaac/Zelda hybrid feel
    for (size_t i = 0; i + 1 < rooms.size(); ++i) {
        RoomConnection fwd;
        fwd.targetRoomId = static_cast<int>(i + 1);
        fwd.side = DoorSide::South;
        fwd.locked = (rnd(seed + static_cast<int>(i), 100) < 20);
        rooms[i].connections.push_back(fwd);

        RoomConnection back;
        back.targetRoomId = static_cast<int>(i);
        back.side = DoorSide::North;
        rooms[i + 1].connections.push_back(back);
    }
}
