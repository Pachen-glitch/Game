#include "SkeletonRenderer.h"

#include "../utils/AssetPaths.h"

#include <cmath>

SkeletonRenderer::SkeletonRenderer() {

    buildAnimations();

    anim.play("idle_down");

    anim.applyToSprite(sprite, 4.f);
}

void SkeletonRenderer::buildAnimations() {

    registerWalkClips();

    registerIdleClips();

    registerAttackClips();

    registerDeathClips();
}

void SkeletonRenderer::registerWalkClips() {

    const char* dirs[] = {
        "down",
        "up",
        "left",
        "right"
    };

    const char* suffix[] = {
        "d",
        "u",
        "l",
        "r"
    };

    for (int d = 0; d < 4; ++d) {

        std::vector<std::string> frames;

        for (int i = 1; i <= 10; ++i) {

            frames.push_back(
                AssetPaths::resolveFirst({
                    "sprites/enemies/skeleton/move/walk_" +
                    std::string(suffix[d]) +
                    std::to_string(i) +
                    ".png"
                })
            );
        }

        Animation walk =
            AssetPaths::buildAnimation(
                frames,
                0.10f,
                true
            );

        anim.registerAnimation(
            std::string("walk_") + dirs[d],
            walk
        );
    }
}

void SkeletonRenderer::registerIdleClips() {

    const char* dirs[] = {
        "down",
        "up",
        "left",
        "right"
    };

    const char* suffix[] = {
        "d",
        "u",
        "l",
        "r"
    };

    for (int d = 0; d < 4; ++d) {

        std::vector<std::string> frames;

        for (int i = 1; i <= 10; ++i) {

            frames.push_back(
                AssetPaths::resolveFirst({
                    "sprites/enemies/skeleton/idle/idle_" +
                    std::string(suffix[d]) +
                    std::to_string(i) +
                    ".png"
                })
            );
        }

        Animation idle =
            AssetPaths::buildAnimation(
                frames,
                0.18f,
                true
            );

        anim.registerAnimation(
            std::string("idle_") + dirs[d],
            idle
        );
    }
}

void SkeletonRenderer::registerAttackClips() {

    const char* dirs[] = {
        "down",
        "up",
        "left",
        "right"
    };

    const char* suffix[] = {
        "d",
        "u",
        "l",
        "r"
    };

    for (int d = 0; d < 4; ++d) {

        std::vector<std::string> frames;

        for (int i = 1; i <= 10; ++i) {

            frames.push_back(
                AssetPaths::resolveFirst({
                    "sprites/enemies/skeleton/attack/attack_" +
                    std::string(suffix[d]) +
                    std::to_string(i) +
                    ".png"
                })
            );
        }

        Animation attack =
            AssetPaths::buildAnimation(
                frames,
                0.08f,
                false
            );

        anim.registerAnimation(
            std::string("attack_") + dirs[d],
            attack
        );
    }
}

void SkeletonRenderer::registerDeathClips() {

    const char* dirs[] = {
        "down",
        "up",
        "left",
        "right"
    };

    const char* suffix[] = {
        "d",
        "u",
        "l",
        "r"
    };

    for (int d = 0; d < 4; ++d) {

        std::vector<std::string> frames;

        for (int i = 1; i <= 10; ++i) {

            frames.push_back(
                AssetPaths::resolveFirst({
                    "sprites/enemies/skeleton/death/death_" +
                    std::string(suffix[d]) +
                    std::to_string(i) +
                    ".png"
                })
            );
        }

        Animation death =
            AssetPaths::buildAnimation(
                frames,
                0.12f,
                false
            );

        anim.registerAnimation(
            std::string("death_") + dirs[d],
            death
        );
    }
}

std::string SkeletonRenderer::clipForState(
    SkeletonEnemy& skeleton
) const {

    std::string dir;

    switch (skeleton.getFacingDirection()) {

        case Direction::UP:
            dir = "up";
            break;

        case Direction::DOWN:
            dir = "down";
            break;

        case Direction::LEFT:
            dir = "left";
            break;

        case Direction::RIGHT:
            dir = "right";
            break;
    }

    if (skeleton.isDeathAnimPending()) {
        return "death_" + dir;
    }

    if (skeleton.getAIState() == EnemyState::Attack) {
        return "attack_" + dir;
    }

    sf::Vector2f vel =
        skeleton.getVelocity();

    float speed =
        std::sqrt(
            vel.x * vel.x +
            vel.y * vel.y
        );

    if (speed > 5.f) {
        return "walk_" + dir;
    }

    return "idle_" + dir;
}

void SkeletonRenderer::update(
    SkeletonEnemy& skeleton,
    float deltaTime
) {

    std::string clip =
        clipForState(skeleton);

    bool restart =
        clip != lastClip;

    if (restart) {

        anim.play(clip);

        lastClip = clip;
    }

    anim.update(deltaTime);

    anim.applyToSprite(sprite, 4.f);

    sprite.setPosition(
        skeleton.getPosition()
    );

    skeleton.getSprite() = sprite;
}

void SkeletonRenderer::draw(
    sf::RenderWindow& window
) {

    window.draw(sprite);
}
