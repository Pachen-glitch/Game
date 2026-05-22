#pragma once

#include "Enemy.h"

class Player;

// Fast flyer — wider aggro, erratic chase.
class BatEnemy : public Enemy {
public:
    BatEnemy(sf::Vector2f pos);
    void think(Player& player, float dt) override;
};
