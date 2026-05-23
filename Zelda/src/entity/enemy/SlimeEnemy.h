#pragma once

#include "Enemy.h"

class Player;
class EntityManager;

// Slow, hop-based slime — idles often, chases only when close.
class SlimeEnemy : public Enemy {
public:
    SlimeEnemy(sf::Vector2f pos);
    void think(Player& player, float dt, const Map& map) override;
    void takeHit(int damage, sf::Vector2f knockback) override;

    static void onDeathSplit(sf::Vector2f pos, EntityManager& manager);

protected:
    void initKindStats() override;
    void modifyVelocity(float dt) override;

private:
    void updateHopTimers(float dt);
    bool hopBurstActive = false;
    Timer hopCooldown;
    Timer hopBurstTimer;
};
