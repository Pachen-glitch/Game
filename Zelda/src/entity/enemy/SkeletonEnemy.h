#pragma once

#include "Enemy.h"

class Player;

// Tankier melee — calls nearby skeletons when hurt.
class SkeletonEnemy : public Enemy {
public:
    SkeletonEnemy(sf::Vector2f pos);
    void think(Player& player, float dt, const Map& map) override;
    void takeHit(int damage, sf::Vector2f knockback) override;
};
