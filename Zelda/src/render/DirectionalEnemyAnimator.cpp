#include "DirectionalEnemyAnimator.h"

#include "../utils/AssetPaths.h"

#include <cmath>

std::string DirectionalEnemyAnimator::directionToString(Direction dir) const {

    switch (dir) {

        case Direction::UP:
            return "arriba";

        case Direction::DOWN:
            return "abajo";

        case Direction::LEFT:
            return "izquierda";

        case Direction::RIGHT:
            return "derecha";
    }

    return "abajo";
}

void DirectionalEnemyAnimator::setupSkeleton() {

    const std::string dirs[] = {
        "arriba",
        "abajo",
        "izquierda",
        "derecha"
    };

    const std::string states[] = {
        "idle",
        "walk",
        "attack"
    };

    for (const auto& dir : dirs) {

        for (const auto& state : states) {

            std::string prefix =
                state + "_" + dir + "_";

            auto frames =
                AssetPaths::collectFramesByPrefix(
                    "enemies/skeleton",
                    prefix
                );

            Animation clip =
                AssetPaths::buildAnimation(
                    frames,
                    0.12f,
                    true
                );

            anim.registerAnimation(
                state + "_" + dir,
                clip
            );
        }
    }

    auto deathFrames =
        AssetPaths::collectFramesByPrefix(
            "enemies/skeleton",
            "death_"
        );

    Animation deathClip =
        AssetPaths::buildAnimation(
            deathFrames,
            0.14f,
            false
        );

    anim.registerAnimation(
        "death",
        deathClip
    );
}

std::string DirectionalEnemyAnimator::pickClip(
    const Enemy& enemy
) const {

    std::string dir =
        directionToString(
            enemy.getFacingDirection()
        );

    if (enemy.isDeathAnimPending()) {
        return "death";
    }

    if (enemy.getAIState() == EnemyState::Attack) {
        return "attack_" + dir;
    }

    sf::Vector2f vel = enemy.getVelocity();

    float speed =
        std::sqrt(vel.x * vel.x + vel.y * vel.y);

    if (speed > 5.f) {
        return "walk_" + dir;
    }

    return "idle_" + dir;
}

void DirectionalEnemyAnimator::update(
    Enemy& enemy,
    float dt
) {

    std::string clip =
        pickClip(enemy);

    if (clip != lastClip) {

        anim.play(
            clip,
            enemy.getAIState() == EnemyState::Attack
        );

        lastClip = clip;
    }

    anim.update(dt);
}

void DirectionalEnemyAnimator::applyToEntity(
    Entity& entity,
    float scale
) {

    anim.applyToSprite(
        entity.getSprite(),
        scale
    );

    entity.getSprite().setPosition(
        entity.getPosition()
    );
}