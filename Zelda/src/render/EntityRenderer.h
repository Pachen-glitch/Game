#pragma once

#include "../entity/base/EntityManager.h"

#include "../entity/enemy/SlimeEnemy.h"
#include "../entity/enemy/SkeletonEnemy.h"

#include "../render/EnemyAnimator.h"
#include "../render/SkeletonRenderer.h"

#include <SFML/Graphics/RenderWindow.hpp>

#include <unordered_map>

class Player;

class EntityRenderer {
public:

    void update(
        EntityManager& entities,
        float dt
    );

    void draw(
        sf::RenderWindow& window,
        const Player& player,
        EntityManager& entities
    );

private:

    std::unordered_map<
        SlimeEnemy*,
        EnemyAnimator
    > slimeAnimators;

    std::unordered_map<
        SkeletonEnemy*,
        SkeletonRenderer
    > skeletonAnimators;
};