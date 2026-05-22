#include "World.h"

#include <cmath>

#include "../core/Constants.h"
#include "../entity/enemy/BatEnemy.h"
#include "../entity/enemy/SkeletonEnemy.h"
#include "../entity/enemy/SlimeEnemy.h"
#include "../entity/enemy/SummonerEnemy.h"
#include "../entity/items/Coin.h"
#include "../entity/items/Door.h"
#include "../entity/items/Heart.h"
#include "../entity/items/Key.h"
#include "../entity/npc/Shopkeeper.h"

#include <cstdlib>

void World::newRun(int seed) {
    rooms = generator.generate(seed);
    currentRoomId = 0;
    entities.clear();
    loadRoom(0);
}

void World::loadRoom(int roomId) {
    if (roomId < 0 || roomId >= static_cast<int>(rooms.size())) return;
    currentRoomId = roomId;
    entities.clear();
    rooms[roomId].visited = true;
    populateRoom();
}

void World::populateRoom() {
    Room& room = currentRoom();
    int w = room.map.getWidth();
    int h = room.map.getHeight();

    auto rndPos = [w, h]() {
        return sf::Vector2f(
            static_cast<float>((2 + std::rand() % (w - 4)) * Constants::TILE_SIZE),
            static_cast<float>((2 + std::rand() % (h - 4)) * Constants::TILE_SIZE)
        );
    };

    switch (room.type) {
        case RoomType::Combat:
        case RoomType::Boss:
            for (int i = 0; i < 3 + std::rand() % 3; ++i) {
                int pick = std::rand() % 4;
                if (pick == 0) entities.spawn<SlimeEnemy>(rndPos());
                else if (pick == 1) entities.spawn<BatEnemy>(rndPos());
                else if (pick == 2) entities.spawn<SkeletonEnemy>(rndPos());
                else {
                    auto* s = entities.spawn<SummonerEnemy>(rndPos());
                    if (s) s->setEntityManager(&entities);
                }
            }
            if (std::rand() % 2) entities.spawn<Coin>(rndPos());
            break;
        case RoomType::Treasure:
            entities.spawn<Key>(rndPos());
            for (int i = 0; i < 5; ++i) entities.spawn<Coin>(rndPos());
            if (std::rand() % 2) entities.spawn<Heart>(rndPos());
            break;
        case RoomType::Shop:
            entities.spawn<Shopkeeper>(room.getPlayerSpawn() + sf::Vector2f(64.f, 0.f));
            break;
        default:
            break;
    }

    for (const auto& conn : room.connections) {
        entities.spawn<Door>(room.getDoorWorldPos(conn.side), conn.locked);
    }
}

Room& World::currentRoom() { return rooms[currentRoomId]; }
const Room& World::currentRoom() const { return rooms[currentRoomId]; }

void World::updateEnemies(Player& player, float dt) {
    for (auto& e : entities.all()) {
        if (!e || !e->isActive() || e->getType() != EntityType::Enemy) continue;
        auto* enemy = dynamic_cast<Enemy*>(e.get());
        if (enemy) enemy->think(player, dt);
    }
    entities.removeInactive();
}

bool World::tryTransition(Player& player, DoorSide& outSide) {
    sf::Vector2f pos = player.getPosition();
    Room& room = currentRoom();

    for (const auto& conn : room.connections) {
        sf::Vector2f doorPos = room.getDoorWorldPos(conn.side);
        float dist = std::hypot(pos.x - doorPos.x, pos.y - doorPos.y);
        if (dist < Constants::TILE_SIZE * 1.2f) {
            loadRoom(conn.targetRoomId);
            player.setPosition(currentRoom().getPlayerSpawn());
            outSide = conn.side;
            return true;
        }
    }
    return false;
}
