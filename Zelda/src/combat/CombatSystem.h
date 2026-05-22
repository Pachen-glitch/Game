#pragma once

#include "Hitbox.h"

#include "../entity/base/EntityManager.h"
#include "../entity/player/Player.h"

#include <vector>

class Enemy;

// Resolves hitboxes vs entities — sword, spin, enemy attacks.
class CombatSystem {
public:
    void clear();

    void addHitbox(const Hitbox& box);
    void spawnSwordHitbox(Player& player);
    void spawnSpinHitbox(Player& player);

    void update(float dt, Player& player, EntityManager& entities);

    const std::vector<Hitbox>& getHitboxes() const { return hitboxes; }

private:
    std::vector<Hitbox> hitboxes;
    int nextOwnerId = 1;

    void resolvePlayerHits(Player& player, EntityManager& entities);
    void resolveEnemyHits(Player& player, EntityManager& entities);
    static sf::FloatRect offsetRect(sf::Vector2f pos, sf::Vector2f size, sf::Vector2f offset);
};
