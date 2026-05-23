#pragma once

#include "Enemy.h"

class Player;
class EntityManager;

// Stays at range and summons slimes periodically.
class SummonerEnemy : public Enemy {
public:
    SummonerEnemy(sf::Vector2f pos);
    void think(Player& player, float dt, const Map& map) override;

    void setEntityManager(EntityManager* mgr) { entities = mgr; }

private:
    EntityManager* entities = nullptr;
    float summonCooldown = 0.f;
};
