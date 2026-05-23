#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <unordered_map>

#include "../entity/base/EntityManager.h"
#include "../entity/enemy/SlimeEnemy.h"
#include "../entity/player/Player.h"
#include "EnemyAnimator.h"

class EntityRenderer {
public:
    void update(EntityManager& entities, float dt);
    void draw(sf::RenderWindow& window, const Player& player, EntityManager& entities);

private:
    std::unordered_map<SlimeEnemy*, EnemyAnimator> slimeAnimators;
};
