#pragma once

#include "../animation/AnimationManager.h"
#include "../entity/enemy/NarutoAttack.h"
#include "../entity/enemy/NarutoBoss.h"
#include "../entity/enemy/NarutoCloneEnemy.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include <string>

class NarutoRenderer {
public:
    NarutoRenderer();

    void update(NarutoBoss& boss, float deltaTime);
    void updateClone(NarutoCloneEnemy& clone, float deltaTime);

    void drawBoss(sf::RenderWindow& window, NarutoBoss& boss);
    void drawClone(sf::RenderWindow& window, NarutoCloneEnemy& clone);

private:
    void buildAnimations();

    std::string clipForBoss(NarutoBoss& boss) const;
    std::string clipForClone(NarutoCloneEnemy& clone) const;

    AnimationManager anim;
    sf::Sprite sprite;
    std::string lastClip;
    NarutoAttack lastTrackedAttack = NarutoAttack::None;
};
