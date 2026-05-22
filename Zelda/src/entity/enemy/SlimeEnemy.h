#pragma once

#include "Enemy.h"

class Player;
class EntityManager;

// Splits into two smaller slimes on death.
class SlimeEnemy : public Enemy {
public:
    SlimeEnemy(sf::Vector2f pos);
    void think(Player& player, float dt) override;

    static void onDeathSplit(sf::Vector2f pos, EntityManager& manager);
};
