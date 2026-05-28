#include "DirectionalEnemyAnimator.h"

#include "../utils/AssetPaths.h"

#include <cmath>

std::string DirectionalEnemyAnimator::directionToString(
    Direction dir
) const {

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
        "attack",
        "death"
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
                    state == "attack" ? 0.08f :
                    state == "death" ? 0.14f :
                    0.12f,

                    state != "death"
                );

            anim.registerAnimation(
                state + "_" + dir,
                clip
            );
        }
    }
}

std::string DirectionalEnemyAnimator::pickClip(
    const Enemy& enemy
) const {

    std::string dir =
        directionToString(
            enemy.getFacingDirection()
        );

    if (enemy.isDeathAnimPending()) {
        return "death_" + dir;
    }

    if (enemy.getAIState() == EnemyState::Attack) {
        return "attack_" + dir;
    }

    sf::Vector2f vel =
        enemy.getVelocity();

    float speed =
        std::sqrt(
            vel.x * vel.x +
            vel.y * vel.y
        );

    // NO diagonal real
    // prioriza eje dominante

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
