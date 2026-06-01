#pragma once

#include "Enemy.h"

class Player;

// Shadow clone — low HP, short life, applies pressure.
class NarutoCloneEnemy : public Enemy {
public:
    NarutoCloneEnemy(sf::Vector2f pos);

    void think(Player& player, float dt, const Map& map) override;
    void update(float dt) override;

    bool isSpawning() const { return spawnTimer > 0.f; }
    float getSpawnProgress() const;

private:
    float lifeTimer = 10.f;
    float spawnTimer = 0.55f;
};
