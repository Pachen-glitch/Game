#pragma once

#include "Enemy.h"

class BatEnemy : public Enemy {
public:
    BatEnemy(sf::Vector2f pos);
    void think(Player& player, float dt, const Map& map) override;

protected:
    void initKindStats() override;
    void modifyVelocity(float dt) override;
};
