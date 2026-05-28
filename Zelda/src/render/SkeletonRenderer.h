#pragma once

#include "../animation/AnimationManager.h"
#include "../entity/enemy/SkeletonEnemy.h"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include <string>

class SkeletonRenderer {
public:

    SkeletonRenderer();

    void update(
        SkeletonEnemy& skeleton,
        float deltaTime
    );

    void draw(
        sf::RenderWindow& window
    );

private:

    void buildAnimations();

    void registerWalkClips();

    void registerIdleClips();

    void registerAttackClips();

    void registerDeathClips();

    std::string clipForState(
        SkeletonEnemy& skeleton
    ) const;

    AnimationManager anim;

    sf::Sprite sprite;

    std::string lastClip;
};