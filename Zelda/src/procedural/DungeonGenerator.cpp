#include "DungeonGenerator.h"
#include "RoomTemplates.h"
#include "../core/Constants.h"

#include <SFML/System/Vector2.hpp>

#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <vector>

static int rnd(int seed, int mod) {// Genera un numero aleatorio
    if (mod <= 0) return 0;
    return (std::abs(seed * 1103515245 + 12345) % mod);
}

// Elige un tipo de room ponderado
RoomType DungeonGenerator::pickWeightedRoom(
    bool needShop, bool needTreasure, int depth
) {
    int roll = rnd(seed + depth, 100); // Genera un numero aleatorio        
    if (needShop && roll < 25) return RoomType::Shop;
    if (needTreasure && roll < 40) return RoomType::Treasure;
    if (roll < 55) return RoomType::Combat;
    if (roll < 75) return RoomType::Corridor;
    return RoomType::Combat;
}

// Genera el dungeon
std::vector<Room> DungeonGenerator::generate(
    int s,
    int floorIndex,
    bool placeBoss
) {
    seed = s == 0 ? static_cast<int>(std::time(nullptr)) : s; // Genera un numero aleatorio        
    seed ^= floorIndex * 991;
    std::srand(seed);

    int roomCount = Constants::MIN_ROOMS + // Numero de rooms
        rnd(seed, Constants::MAX_ROOMS - Constants::MIN_ROOMS + 1);

    std::vector<Room> rooms; // Vector de rooms
    rooms.reserve(roomCount);

    std::set<std::pair<int, int>> used; // Set de rooms usadas  
    std::queue<std::pair<int, int>> frontier;
    frontier.push({0, 0});
    used.insert({0, 0});

    bool shopPlaced = false;
    bool treasurePlaced = false;

    for (int i = 0; i < roomCount; ++i) {
        if (frontier.empty()) {
            int nx = rooms.empty() ? 0 : rooms.back().gridPos.x + 1;
            int ny = rooms.empty() ? 0 : rooms.back().gridPos.y;// Posicion y de la ultima room
            while (used.count({nx, ny})) {
                nx++;
                if (nx > 32) { nx = 0; ny++; } // Si el numero de x es mayor a 32, se establece x a 0 y se incrementa y
            }
            used.insert({nx, ny});
            frontier.push({nx, ny});
        }

        Room room;
        room.id = i;
        auto gp = frontier.front();
        room.gridPos = sf::Vector2i(gp.first, gp.second);
        frontier.pop();// Elimina la room de la frontera    

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

    if (placeBoss) {// Si se debe colocar el boss
        int bossId = assignBossRoom(rooms); // Asigna el boss a la room
        rooms[bossId].type = RoomType::Boss;
        int antechamberId = pickBossAntechamberId(rooms, bossId);
        markBossGates(rooms, bossId, antechamberId);
        validateBossGeneration(rooms, bossId, antechamberId);
    }

    buildAllLayouts(rooms);
    return rooms;
}

void DungeonGenerator::connectRooms(std::vector<Room>& rooms) {
    std::map<std::pair<int, int>, int> gridToId; // Map de rooms        
    for (const auto& room : rooms) {
        gridToId[{room.gridPos.x, room.gridPos.y}] = room.id;
    }

    const int dirs[4][2] = {{0, -1}, {0, 1}, {1, 0}, {-1, 0}};
    const DoorSide sides[4] = {
        DoorSide::North, DoorSide::South, DoorSide::East, DoorSide::West
    };

    for (auto& room : rooms) {// Limpia las conexiones de la room
        room.connections.clear();// Limpia las conexiones de la room
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

int DungeonGenerator::assignBossRoom(std::vector<Room>& rooms) const {// Asigna el boss a la room
    if (rooms.empty()) return 0;

    sf::Vector2i start = rooms.front().gridPos; // Posicion de la primera room
    int bestId = static_cast<int>(rooms.size()) - 1;
    int bestDist = -1; // Distancia mejor

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

namespace {

bool isEligibleAntechamber(const Room& room, int bossRoomId) {
    return room.id != bossRoomId
        && room.type != RoomType::Shop
        && room.type != RoomType::Start;
}

int countBossGates(const std::vector<Room>& rooms) {
    int count = 0;
    for (const auto& room : rooms) {
        for (const auto& conn : room.connections) {
            if (conn.isBossGate) {
                ++count;
            }
        }
    }
    return count;
}

} // namespace

int DungeonGenerator::pickBossAntechamberId(
    const std::vector<Room>& rooms,
    int bossRoomId
) const {
    if (rooms.empty() || bossRoomId < 0 ||
        bossRoomId >= static_cast<int>(rooms.size())) {
        return -1;
    }

    const int roomCount = static_cast<int>(rooms.size());
    std::vector<int> parent(roomCount, -1);
    std::vector<bool> visited(roomCount, false);
    std::queue<int> frontier;
    frontier.push(0);
    visited[0] = true;

    while (!frontier.empty()) {
        const int roomId = frontier.front();
        frontier.pop();

        for (const auto& conn : rooms[roomId].connections) {
            const int nextId = conn.targetRoomId;
            if (nextId < 0 || nextId >= roomCount || visited[nextId]) {
                continue;
            }
            visited[nextId] = true;
            parent[nextId] = roomId;
            frontier.push(nextId);
        }
    }

    if (visited[bossRoomId]) {
        int candidate = parent[bossRoomId];
        while (candidate >= 0) {
            if (isEligibleAntechamber(rooms[candidate], bossRoomId)) {
                return candidate;
            }
            candidate = parent[candidate];
        }
    }

    int fallbackId = -1;
    int fallbackDist = -1;
    const sf::Vector2i startPos = rooms.front().gridPos;

    for (const auto& conn : rooms[bossRoomId].connections) {
        const int neighborId = conn.targetRoomId;
        if (neighborId < 0 || neighborId >= roomCount) continue;
        const Room& neighbor = rooms[neighborId];
        if (!isEligibleAntechamber(neighbor, bossRoomId)) continue;

        const int dist = std::abs(neighbor.gridPos.x - startPos.x)
            + std::abs(neighbor.gridPos.y - startPos.y);
        if (fallbackId < 0 || dist > fallbackDist) {
            fallbackId = neighborId;
            fallbackDist = dist;
        }
    }

    return fallbackId;
}

void DungeonGenerator::markBossGates(
    std::vector<Room>& rooms,
    int bossRoomId,
    int antechamberId
) {
    if (antechamberId < 0 || antechamberId >= static_cast<int>(rooms.size())) {
        return;
    }

    rooms[antechamberId].type = RoomType::BossAntechamber;

    for (auto& conn : rooms[antechamberId].connections) {
        if (conn.targetRoomId != bossRoomId) continue;

        conn.isBossGate = true;
        conn.locked = true;
        break;
    }
}

void DungeonGenerator::isolateBossRoom(
    std::vector<Room>& rooms,
    int bossRoomId,
    int antechamberId
) {
    if (bossRoomId < 0 || bossRoomId >= static_cast<int>(rooms.size())) {
        return;
    }

    rooms[bossRoomId].connections.clear();

    for (auto& room : rooms) {
        if (room.id == bossRoomId) continue;

        auto& conns = room.connections;
        conns.erase(
            std::remove_if(
                conns.begin(),
                conns.end(),
                [&](const RoomConnection& conn) {
                    if (conn.targetRoomId != bossRoomId) return false;
                    return !(room.id == antechamberId && conn.isBossGate);
                }
            ),
            conns.end()
        );
    }
}

void DungeonGenerator::validateBossGeneration(
    std::vector<Room>& rooms,
    int bossRoomId,
    int antechamberId
) {
    int bossRooms = 0;
    int antechambers = 0;

    for (auto& room : rooms) {
        if (room.type == RoomType::Boss) {
            ++bossRooms;
            if (room.id != bossRoomId) {
                room.type = RoomType::Combat;
            }
        } else if (room.type == RoomType::BossAntechamber) {
            ++antechambers;
            if (room.id != antechamberId) {
                room.type = RoomType::Combat;
            }
        }
    }

    if (antechamberId >= 0 &&
        antechamberId < static_cast<int>(rooms.size()) &&
        rooms[antechamberId].type != RoomType::BossAntechamber) {
        rooms[antechamberId].type = RoomType::BossAntechamber;
    }

    for (auto& room : rooms) {
        for (auto& conn : room.connections) {
            if (!conn.isBossGate) continue;

            const bool keepGate = room.id == antechamberId
                && conn.targetRoomId == bossRoomId;
            if (!keepGate) {
                conn.isBossGate = false;
                conn.locked = false;
            }
        }
    }

    if (antechamberId >= 0 &&
        antechamberId < static_cast<int>(rooms.size()) &&
        countBossGates(rooms) == 0) {
        for (auto& conn : rooms[antechamberId].connections) {
            if (conn.targetRoomId != bossRoomId) continue;
            conn.isBossGate = true;
            conn.locked = true;
            break;
        }
    }

    isolateBossRoom(rooms, bossRoomId, antechamberId);

    bossRooms = 0;
    antechambers = 0;
    for (const auto& room : rooms) {
        if (room.type == RoomType::Boss) ++bossRooms;
        if (room.type == RoomType::BossAntechamber) ++antechambers;
    }

    const int bossGates = countBossGates(rooms);
    const int bossRoomConnections = bossRoomId >= 0 &&
        bossRoomId < static_cast<int>(rooms.size())
        ? static_cast<int>(rooms[bossRoomId].connections.size())
        : -1;

    std::cerr << "[Dungeon]\n"
              << "BossRooms = " << bossRooms << "\n"
              << "BossAntechambers = " << antechambers << "\n"
              << "BossGates = " << bossGates << "\n"
              << "BossRoomConnections = " << bossRoomConnections << "\n";
}

void DungeonGenerator::buildAllLayouts(std::vector<Room>& rooms) {// Construye todas las layouts
    for (auto& room : rooms) {
        int salt = seed + room.id * 17 + room.gridPos.x * 31 + room.gridPos.y * 53; // Sal de la room
        room.buildLayout(salt);
    }
}
