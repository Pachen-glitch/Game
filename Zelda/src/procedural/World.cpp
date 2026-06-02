#include "World.h"



#include <cmath>



#include "../core/Constants.h"

#include "../audio/AudioManager.h"

#include "../entity/enemy/SlimeEnemy.h"

#include "../entity/enemy/BatEnemy.h"

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
#include <vector>

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
    inVictoryRoom = false;
    victoryDoorSpawned = false;

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
    inVictoryRoom = false;

    entities.clear();

    rooms[roomId].visited = true;

    populateRoom();

    if (rooms[roomId].type == RoomType::Shop) {
        AudioManager::instance().playShopMusic();
    }
    else if (rooms[roomId].type == RoomType::BossAntechamber) {
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

            entities.spawn<BatEnemy>(rndPos());

            break;



        case RoomType::Combat:

        case RoomType::Corridor: {
            const int enemyCount = 2 + std::rand() % 5;

            for (int i = 0; i < enemyCount; ++i) {
                switch (std::rand() % 4) {
                    case 0: entities.spawn<SlimeEnemy>(rndPos()); break;
                    case 1: entities.spawn<SkeletonEnemy>(rndPos()); break;
                    case 2: entities.spawn<SummonerEnemy>(rndPos()); break;
                    case 3: entities.spawn<BatEnemy>(rndPos()); break;
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
    if (inVictoryRoom) {
        return victoryRoom;
    }
    return rooms[currentRoomId];
}

const Room& World::currentRoom() const {
    if (inVictoryRoom) {
        return victoryRoom;
    }
    return rooms[currentRoomId];
}



bool World::hasActiveNarutoBoss() const {

    for (const auto& e : entities.all()) {

        if (!e || !e->isActive()) continue;

        if (dynamic_cast<const NarutoBoss*>(e.get())) return true;

    }

    return false;

}

void World::buildVictoryRoom() {
    if (victoryRoomBuilt) return;

    static const std::vector<std::string> kVictoryLayout = {
        "####################",
        "#..................#",
        "#..................#",
        "#..................#",
        "#..................#",
        "#..................#",
        "#..................#",
        "#..................#",
        "#..................#",
        "#..................#",
        "#..................#",
        "#..................#",
        "#..................#",
        "#..................#",
        "#..................#",
        "#..................#",
        "#..................#",
        "#..................#",
        "#..................#",
        "####################"
    };

    victoryRoom.id = -1;
    victoryRoom.type = RoomType::Victory;
    victoryRoom.cleared = true;
    victoryRoom.visited = true;
    victoryRoom.templateId = "victory";

    for (int y = 0; y < static_cast<int>(kVictoryLayout.size()); ++y) {
        for (int x = 0; x < static_cast<int>(kVictoryLayout[y].size()); ++x) {
            char c = kVictoryLayout[static_cast<size_t>(y)][static_cast<size_t>(x)];
            TileType tile = (c == '#') ? TileType::WALL : TileType::FLOOR;
            victoryRoom.map.setTile(x, y, tile);
        }
    }

    victoryRoomBuilt = true;
}

void World::spawnVictoryDoor() {
    if (victoryDoorSpawned || inVictoryRoom) return;
    if (currentRoom().type != RoomType::Boss) return;

    Room& room = currentRoom();
    entities.spawn<Door>(room.getBossGateWorldPos(), false, DoorKind::Victory);
    victoryDoorSpawned = true;
}

void World::trySpawnVictoryDoorAfterBossDefeat() {
    if (victoryDoorSpawned || inVictoryRoom) return;
    if (currentRoom().type != RoomType::Boss) return;
    if (hasActiveNarutoBoss()) return;

    Room& room = currentRoom();
    if (!room.cleared) {
        room.cleared = true;
        if (RunScoreTracker* tracker = RunScoreTracker::active()) {
            tracker->onRoomCleared(room.id);
        }
    }

    spawnVictoryDoor();
}

bool World::tryEnterVictoryRoom(Player& player) {
    if (inVictoryRoom || !victoryDoorSpawned) return false;
    if (currentRoom().type != RoomType::Boss) return false;

    for (auto& e : entities.all()) {
        if (!e || !e->isActive()) continue;

        auto* door = dynamic_cast<Door*>(e.get());
        if (!door || !door->isVictoryDoor()) continue;
        if (!door->getBounds().intersects(player.getBounds())) continue;

        buildVictoryRoom();
        inVictoryRoom = true;
        entities.clear();
        player.setPosition(victoryRoom.getPlayerSpawn());
        return true;
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

int World::debugKillNormalEnemies() {
    auto shouldDropHeart = [](const Enemy& enemy) {
        switch (enemy.getKind()) {
            case EnemyKind::Slime:
            case EnemyKind::Skeleton:
            case EnemyKind::Summoner:
                return std::rand() % 100 < 25;
            default:
                return false;
        }
    };

    int killed = 0;
    std::vector<sf::Vector2f> pendingHearts;

    for (auto& e : entities.all()) {
        if (!e || !e->isActive() || e->getType() != EntityType::Enemy) continue;
        if (dynamic_cast<NarutoBoss*>(e.get())) continue;

        auto* enemy = dynamic_cast<Enemy*>(e.get());
        if (!enemy || enemy->isDead() || enemy->isDeathAnimPending()) continue;

        const sf::Vector2f deathPos = enemy->getPosition();
        const bool wasAlive = !enemy->isDeathAnimPending();
        enemy->takeHit(9999, {0.f, 0.f});
        ++killed;

        if (wasAlive && enemy->isDeathAnimPending() && shouldDropHeart(*enemy)) {
            pendingHearts.push_back(deathPos + sf::Vector2f(8.f, 8.f));
        }
    }

    for (const sf::Vector2f& pos : pendingHearts) {
        entities.spawn<Heart>(pos);
    }

    entities.removeInactive();
    return killed;
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

        if (room.type == RoomType::Boss) {
            spawnVictoryDoor();
        } else {
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

    const int fromRoomId = currentRoomId;



    for (const auto& conn : room.connections) {

        if (!canTraverseConnection(room, conn)) continue;



        DoorSide openingSide = conn.side;

        if (room.type == RoomType::BossAntechamber && conn.isBossGate) {

            openingSide = room.getAntechamberBossPassageSide();

        }

        if (!playerAtOpening(player, room, openingSide)) continue;



        loadRoom(conn.targetRoomId);

        transitionCooldown = 0.5f;



        DoorSide spawnSide = conn.side;

        if (rooms[fromRoomId].type == RoomType::BossAntechamber && conn.isBossGate) {

            for (const auto& backConn : currentRoom().connections) {

                if (backConn.targetRoomId == fromRoomId && backConn.isBossGate) {

                    spawnSide = backConn.side;

                    break;

                }

            }

        }



        player.setPosition(currentRoom().getTransitionSpawn(spawnSide));

        outSide = conn.side;

        return true;

    }



    return false;

}


