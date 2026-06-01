#include "DungeonGenerator.h"
#include "RoomTemplates.h"
#include "../core/Constants.h"

#include <SFML/System/Vector2.hpp>

#include <cstdlib>
#include <ctime>
#include <map>
#include <queue>
#include <set>

static int rnd(int seed, int mod) {
    if (mod <= 0) return 0;
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

std::vector<Room> DungeonGenerator::generate(
    int s,
    int floorIndex,
    bool placeBoss
) {
    seed = s == 0 ? static_cast<int>(std::time(nullptr)) : s;
    seed ^= floorIndex * 991;
    std::srand(seed);

    int roomCount = Constants::MIN_ROOMS +
        rnd(seed, Constants::MAX_ROOMS - Constants::MIN_ROOMS + 1);

    std::vector<Room> rooms;
    rooms.reserve(roomCount);

    std::set<std::pair<int, int>> used;
    std::queue<std::pair<int, int>> frontier;
    frontier.push({0, 0});
    used.insert({0, 0});

    bool shopPlaced = false;
    bool treasurePlaced = false;

    for (int i = 0; i < roomCount; ++i) {
        if (frontier.empty()) {
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
        } else {
            room.type = pickWeightedRoom(!shopPlaced, !treasurePlaced, i);
            if (room.type == RoomType::Shop) shopPlaced = true;
            if (room.type == RoomType::Treasure) treasurePlaced = true;
        }

        rooms.push_back(room);

        const int dirs[4][2] = {{0, -1}, {0, 1}, {1, 0}, {-1, 0}};
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

    if (placeBoss) {
        int bossId = assignBossRoom(rooms);
        rooms[bossId].type = RoomType::Boss;
        markBossGates(rooms, bossId);
    }

    buildAllLayouts(rooms);
    return rooms;
}

void DungeonGenerator::connectRooms(std::vector<Room>& rooms) {
    std::map<std::pair<int, int>, int> gridToId;
    for (const auto& room : rooms) {
        gridToId[{room.gridPos.x, room.gridPos.y}] = room.id;
    }

    const int dirs[4][2] = {{0, -1}, {0, 1}, {1, 0}, {-1, 0}};
    const DoorSide sides[4] = {
        DoorSide::North, DoorSide::South, DoorSide::East, DoorSide::West
    };

    for (auto& room : rooms) {
        room.connections.clear();
        for (int d = 0; d < 4; ++d) {
            int nx = room.gridPos.x + dirs[d][0];
            int ny = room.gridPos.y + dirs[d][1];
            auto it = gridToId.find({nx, ny});
            if (it == gridToId.end()) continue;

            RoomConnection conn;
            conn.targetRoomId = it->second;
            conn.side = sides[d];
            conn.locked = false;
            conn.isBossGate = false;
            room.connections.push_back(conn);
        }
    }
}

int DungeonGenerator::assignBossRoom(std::vector<Room>& rooms) const {
    if (rooms.empty()) return 0;

    sf::Vector2i start = rooms.front().gridPos;
    int bestId = static_cast<int>(rooms.size()) - 1;
    int bestDist = -1;

    for (const auto& room : rooms) {
        if (room.type == RoomType::Shop) continue;
        int dist = std::abs(room.gridPos.x - start.x) +
                   std::abs(room.gridPos.y - start.y);
        if (dist > bestDist) {
            bestDist = dist;
            bestId = room.id;
        }
    }

    return bestId;
}

void DungeonGenerator::markBossGates(std::vector<Room>& rooms, int bossRoomId) {
    for (auto& room : rooms) {
        for (auto& conn : room.connections) {
            if (conn.targetRoomId == bossRoomId) {
                conn.isBossGate = true;
                conn.locked = true;
            }
        }
    }
}

void DungeonGenerator::buildAllLayouts(std::vector<Room>& rooms) {
    for (auto& room : rooms) {
        int salt = seed + room.id * 17 + room.gridPos.x * 31 + room.gridPos.y * 53;
        room.buildLayout(salt);
    }
}
