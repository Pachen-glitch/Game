#pragma once

#include "../animation/AnimationManager.h"
#include "../entity/base/Entity.h"
#include "../entity/enemy/Enemy.h"

#include <string>

class EnemyAnimator {
public:

    void setupSlime();
    void setupSkeleton();
    void setupSummoner();
    void setupBat();

    void update(Enemy& enemy, float dt);

    void applyToEntity(
        Entity& entity,
        float scale = 2.f
    );

    bool isDeathFinished() const {
        return deathFinished;
    }

private:

    std::string pickClip(
        const Enemy& enemy
    ) const;

    AnimationManager anim;

    std::string lastClip;

    bool deathFinished = false;
};