#include "World.h"

#include <cmath>

#include "../core/Constants.h"
#include "../entity/enemy/BatEnemy.h"
#include "../entity/enemy/SkeletonEnemy.h"
#include "../entity/enemy/SlimeEnemy.h"
#include "../entity/enemy/SummonerEnemy.h"
#include "../entity/enemy/NarutoBoss.h"
#include "../entity/enemy/NarutoCloneEnemy.h"
#include "../entity/enemy/Enemy.h"
#include "../entity/items/Coin.h"
#include "../entity/items/Door.h"
#include "../entity/items/Heart.h"
#include "../entity/items/Key.h"
#include "../entity/npc/Shopkeeper.h"
#include "../entity/items/Chest.h"

#include <cstdlib>

void World::newRun(int seed) {
    rooms = generator.generate(seed);
    currentRoomId = 0;
    entities.clear();
    loadRoom(0);
}

void World::loadRoom(int roomId) {

    if (roomId < 0 ||
        roomId >= static_cast<int>(rooms.size()))
        return;

    currentRoomId = roomId;

    entities.clear();

    rooms[roomId].visited = true;

    populateRoom();
}

void World::populateRoom() {

    Room& room = currentRoom();

    int w = room.map.getWidth();
    int h = room.map.getHeight();

    auto rndPos = [&room, w, h]() {

        while (true) {

            int tx = 2 + std::rand() % (w - 4);
            int ty = 2 + std::rand() % (h - 4);

            if (room.map.isWalkable(tx, ty)) {

                return sf::Vector2f(
                    static_cast<float>(tx * Constants::TILE_SIZE),
                    static_cast<float>(ty * Constants::TILE_SIZE)
                );
            }
        }
    };

    switch (room.type) {

        case RoomType::Start:
            entities.spawn<SlimeEnemy>(rndPos());
            entities.spawn<SlimeEnemy>(rndPos());
            entities.spawn<Heart>(rndPos());
            entities.spawn<Key>(rndPos());
            break;
        case RoomType::Combat:
        case RoomType::Boss:
            for (int i = 0; i < 2 + std::rand() % 2; ++i) {

                int enemyType = std::rand() % 4;

                switch (enemyType) {

                    case 0:
                        entities.spawn<SlimeEnemy>(rndPos());
                        break;

                    case 1:
                        entities.spawn<BatEnemy>(rndPos());
                        break;

                    case 2:
                        entities.spawn<SkeletonEnemy>(rndPos());
                        break;

                    case 3:
                        entities.spawn<SummonerEnemy>(rndPos());
                        break;
                }
            }

            
            if (std::rand() % 2) entities.spawn<Heart>(rndPos());
            if (std::rand() % 2) entities.spawn<Key>(rndPos());
            if (std::rand() % 2) entities.spawn<Coin>(rndPos());
            break;

        case RoomType::Boss: {
            sf::Vector2f center(
                static_cast<float>(w * Constants::TILE_SIZE) * 0.5f - 24.f,
                static_cast<float>(h * Constants::TILE_SIZE) * 0.5f - 24.f
            );
            auto* boss = entities.spawn<NarutoBoss>(center);
            boss->setEntityManager(&entities);
            break;
        }

        case RoomType::Treasure:

            entities.spawn<Key>(rndPos());

            entities.spawn<Heart>(rndPos());
            for (int i = 0; i < 3; ++i) entities.spawn<Coin>(rndPos());
            break;

        case RoomType::Shop:
            entities.spawn<Shopkeeper>(room.getPlayerSpawn() + sf::Vector2f(64.f, 0.f));
            break;

        default:
            break;
    }

        if (
        room.cleared ||
        room.type == RoomType::Start ||
        room.type == RoomType::Shop
    ) {

    for (const auto& conn : room.connections) {
        entities.spawn<Door>(room.getDoorWorldPos(conn.side), conn.locked);
    }
}
}

Room& World::currentRoom() {
    return rooms[currentRoomId];
}

const Room& World::currentRoom() const {
    return rooms[currentRoomId];
}

bool World::hasActiveNarutoBoss() const {
    for (const auto& e : entities.all()) {
        if (!e || !e->isActive()) continue;
        if (dynamic_cast<const NarutoBoss*>(e.get())) {
            return true;
        }
    }
    return false;
}

bool World::debugSpawnNarutoNear(sf::Vector2f pos) {
    if (hasActiveNarutoBoss()) {
        return false;
    }

    auto* boss = entities.spawn<NarutoBoss>(pos);
    boss->setEntityManager(&entities);
    return true;
}

bool World::debugRemoveNaruto() {
    bool removed = false;

    for (auto& e : entities.all()) {
        if (!e || !e->isActive()) continue;

        if (auto* boss = dynamic_cast<NarutoBoss*>(e.get())) {
            boss->despawnAllClones();
            boss->deactivate();
            removed = true;
            continue;
        }

        if (dynamic_cast<NarutoCloneEnemy*>(e.get())) {
            e->deactivate();
        }
    }

    entities.removeInactive();
    return removed;
}

void World::updateEnemies(Player& player, float dt, const Map& map) {
    Room& room = currentRoom();
    const bool bossRoom = room.type == RoomType::Boss;

    for (auto& e : entities.all()) {
        if (!e || !e->isActive() || e->getType() != EntityType::Enemy) continue;

        if (auto* boss = dynamic_cast<NarutoBoss*>(e.get())) {
            boss->setEntityManager(&entities);
            boss->think(player, dt, map);
            continue;
        }

        if (auto* clone = dynamic_cast<NarutoCloneEnemy*>(e.get())) {
            clone->think(player, dt, map);
            continue;
        }

        auto* enemy = dynamic_cast<Enemy*>(e.get());
        if (enemy) {
            enemy->think(player, dt, map);
        }
    }

    entities.removeInactive();

    bool roomClearedNow = false;

    if (bossRoom) {
        bool bossAlive = false;
        for (auto& e : entities.all()) {
            if (!e || !e->isActive()) continue;
            if (dynamic_cast<NarutoBoss*>(e.get())) {
                bossAlive = true;
                break;
            }
        }
        roomClearedNow = !bossAlive;
    } else {
        bool hasEnemies = false;
        for (auto& e : entities.all()) {
            if (!e || !e->isActive() || e->getType() != EntityType::Enemy) continue;
            hasEnemies = true;
            break;
        }
        roomClearedNow = !hasEnemies;
    }

    if (roomClearedNow && !room.cleared) {

        room.cleared = true;
        for (const auto& conn : room.connections) {
            entities.spawn<Door>(
                room.getDoorWorldPos(conn.side),
                conn.locked
            );
        }
        // NO poner cofres en tienda
        if (
            room.type != RoomType::Shop &&
            room.type != RoomType::Start
        ) {

            entities.spawn<Chest>(

                room.getPlayerSpawn()
                    + sf::Vector2f(180.f, 0.f),

                &entities
            );
        }
    }
}



bool World::tryTransition(Player& player, DoorSide& outSide)
{
    sf::Vector2f pos = player.getPosition();

    if (transitionCooldown > 0.f)
    {
        transitionCooldown -= 1.f / 60.f;
        return false;
    }

    Room& room = currentRoom();

    for (const auto& conn : room.connections)
    {
        sf::Vector2f doorPos = room.getDoorWorldPos(conn.side);

        float dist = std::hypot(
            pos.x - doorPos.x,
            pos.y - doorPos.y
        );

        if (dist < Constants::TILE_SIZE * 1.2f)
        {
            loadRoom(conn.targetRoomId);

            transitionCooldown = 0.5f;

            player.setPosition(
                currentRoom().getTransitionSpawn(conn.side)
            );

            outSide = conn.side;

            return true;
        }
    }

    return false;
}