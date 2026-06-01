#pragma once

#include "Enemy.h"

#include <SFML/Graphics/Rect.hpp>

class Player;

// Shadow clone — low HP, short life, applies pressure.
class NarutoCloneEnemy : public Enemy {
public:
    NarutoCloneEnemy(sf::Vector2f pos);

    void think(Player& player, float dt, const Map& map) override;
    void update(float dt) override;

    bool isSpawning() const { return spawnTimer > 0.f; }
    bool isSmokeVisible() const { return spawnTimer > 0.f; }
    bool isVanishing() const { return vanishTimer > 0.f; }
    bool isKickActive() const { return kickActive; }
    bool isAttacking() const { return attackTimer > 0.f; }
    float getSpawnProgress() const;

    sf::FloatRect getContactBounds() const override;

    void takeHit(int damage, sf::Vector2f knockback) override;

private:
    void startVanish();
    void startKick();
    void updateKick(const Player& player, float dt, const Map& map);
    void finishKick();

    float lifeTimer = 10.f;
    float spawnTimer = 0.72f;
    float vanishTimer = 0.f;
    float attackTimer = 0.f;
    float kickCooldown = 0.f;
    bool kickActive = false;
};
