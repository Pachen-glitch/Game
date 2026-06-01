#include "World.h"



#include <cmath>



#include "../core/Constants.h"

#include "../audio/AudioManager.h"

#include "../entity/enemy/SlimeEnemy.h"

#include "../entity/enemy/SkeletonEnemy.h"

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

#include "../save/RunScoreTracker.h"



#include <cstdlib>

// Genera el dungeon actual

void World::generateCurrentFloor(int seed) {

    const bool placeBoss = true;

    rooms = generator.generate(seed, currentFloor, placeBoss);



    bossRoomId = -1;

    for (const auto& room : rooms) {

        if (room.type == RoomType::Boss) {

            bossRoomId = room.id;

            break;

        }

    }

}

// Inicia un nuevo run

void World::newRun(int seed) {

    runSeed = seed;

    currentFloor = 1;

    maxFloors = Constants::MAX_FLOORS;

    bossGateUnlocked = false;

    generateCurrentFloor(seed);

    currentRoomId = 0;

    entities.clear();

    loadRoom(0);

}


// Carga una room
void World::loadRoom(int roomId) {

    if (roomId < 0 || roomId >= static_cast<int>(rooms.size())) return;

    if (!rooms[roomId].visited) {
        if (RunScoreTracker* tracker = RunScoreTracker::active()) {
            tracker->onRoomFirstEnter(roomId);
        }
    }

    currentRoomId = roomId;

    entities.clear();

    rooms[roomId].visited = true;

    populateRoom();

    if (rooms[roomId].type == RoomType::BossAntechamber) {
        AudioManager::instance().playBossPreBattleMusic();
    } else {
        AudioManager::instance().resumeGameplayMusic();
    }

}


// Desbloquea la puerta del boss
void World::unlockBossGate() {

    if (bossGateUnlocked) return;

    bossGateUnlocked = true;



    for (auto& room : rooms) {

        for (auto& conn : room.connections) {

            if (conn.isBossGate) {

                conn.locked = false;

            }

        }

        room.applyConnectionTiles(true);

    }



    for (auto& e : entities.all()) {

        if (!e || !e->isActive()) continue;

        if (auto* door = dynamic_cast<Door*>(e.get())) {

            if (door->isBossGate()) {

                e->deactivate();

            }

        }

    }

    entities.removeInactive();

}


// Spawna las puertas de conexion
void World::spawnConnectionDoors(Room& room) {

    for (const auto& conn : room.connections) {

        if (!conn.isBossGate) continue;



        sf::Vector2f pos = room.type == RoomType::BossAntechamber
            ? room.getBossGateWorldPos()
            : room.getOpeningWorldPos(conn.side);

        bool locked = !bossGateUnlocked;

        entities.spawn<Door>(pos, locked, DoorKind::BossGate);

    }

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

        case RoomType::Corridor: {
            const int enemyCount = 2 + std::rand() % 5;

            for (int i = 0; i < enemyCount; ++i) {
                switch (std::rand() % 3) {
                    case 0: entities.spawn<SlimeEnemy>(rndPos()); break;
                    case 1: entities.spawn<SkeletonEnemy>(rndPos()); break;
                    case 2: entities.spawn<SummonerEnemy>(rndPos()); break;
                }
            }

            if (std::rand() % 2) entities.spawn<Heart>(rndPos());
            if (std::rand() % 2) entities.spawn<Key>(rndPos());
            if (std::rand() % 2) entities.spawn<Coin>(rndPos());
            break;
        }


// Cleara la room del boss
        case RoomType::BossAntechamber:

            room.cleared = true;

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

            entities.spawn<Shopkeeper>(
                room.getPlayerSpawn() + sf::Vector2f(64.f, 0.f)
            );
            entities.spawn<Chest>(
                room.getPlayerSpawn() + sf::Vector2f(-120.f, 0.f),
                &entities,
                true
            );
            break;



        default:

            break;

    }



    if (room.exitsAreOpen() || room.cleared) {

        spawnConnectionDoors(room);

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

        if (dynamic_cast<const NarutoBoss*>(e.get())) return true;

    }

    return false;

}



bool World::debugSpawnNarutoNear(sf::Vector2f pos) {

    if (hasActiveNarutoBoss()) return false;

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



        if (auto* enemy = dynamic_cast<Enemy*>(e.get())) {

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

        if (RunScoreTracker* tracker = RunScoreTracker::active()) {
            tracker->onRoomCleared(room.id);
        }

        spawnConnectionDoors(room);



        if (room.type != RoomType::Shop && room.type != RoomType::Start &&
            room.type != RoomType::BossAntechamber) {

            entities.spawn<Chest>(

                room.getPlayerSpawn() + sf::Vector2f(180.f, 0.f),

                &entities

            );

        }

    }

}



bool World::canTraverseConnection(

    const Room& room,

    const RoomConnection& conn

) const {

    if (conn.isBossGate) {

        return bossGateUnlocked;

    }

    return room.exitsAreOpen();

}



bool World::playerAtOpening(

    const Player& player,

    const Room& room,

    DoorSide side

) const {

    sf::FloatRect opening = room.getOpeningBounds(side);

    sf::FloatRect pb = player.getBounds();

    return opening.intersects(pb);

}



bool World::tryTransition(Player& player, DoorSide& outSide) {

    if (transitionCooldown > 0.f) {

        transitionCooldown -= 1.f / 60.f;

        return false;

    }



    Room& room = currentRoom();



    for (const auto& conn : room.connections) {

        if (!canTraverseConnection(room, conn)) continue;

        if (!playerAtOpening(player, room, conn.side)) continue;



        loadRoom(conn.targetRoomId);

        transitionCooldown = 0.5f;

        player.setPosition(currentRoom().getTransitionSpawn(conn.side));

        outSide = conn.side;

        return true;

    }



    return false;

}


