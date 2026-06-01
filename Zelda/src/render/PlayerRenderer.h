#pragma once

#include <SFML/Graphics.hpp>

#include "../animation/AnimationManager.h"
#include "../entity/player/Player.h"

class PlayerRenderer {
public:
    PlayerRenderer();

    void update(Player& player, float deltaTime);
    void draw(sf::RenderWindow& window);

private:
    sf::Sprite sprite;
    AnimationManager anim;
    std::string lastClip;
    float effectTime = 0.f;

    void buildAnimations();
    std::string clipForState(Player& player) const;
    void registerWalkClips();
    void registerIdleClips();
    void registerAttackClips();
};
