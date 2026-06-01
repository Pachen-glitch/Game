#pragma once

#include "../animation/AnimationManager.h"
#include "../entity/base/Entity.h"
#include "../entity/enemy/Enemy.h"

#include <string>

class DirectionalEnemyAnimator {
public:

    void setupSkeleton();

    void update(Enemy& enemy, float dt);

    void applyToEntity(
        Entity& entity,
        float scale = 2.f
    );

private:

    std::string pickClip(
        const Enemy& enemy
    ) const;

    std::string directionToString(
        Direction dir
    ) const;

    AnimationManager anim;

    std::string lastClip;
};