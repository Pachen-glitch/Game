#include "SkeletonRenderer.h"

#include "../utils/AssetPaths.h"

#include <cmath>

SkeletonRenderer::SkeletonRenderer() {

    buildAnimations();

    anim.play("idle_down");

    // mismo tamaño que player
    anim.applyToSprite(sprite, 2.f);
}

void SkeletonRenderer::buildAnimations() {// Construye las animaciones

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

        for (int i = 1; i <= 4; ++i) {

            frames.push_back(
                AssetPaths::resolveFirst({
                    "sprites/enemies/skeleton/walk/walk_" +
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

        for (int i = 1; i <= 4; ++i) {

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

    // SOLO izquierda y derecha

    const char* dirs[] = {
        "left",
        "right"
    };

    const char* suffix[] = {
        "l",
        "r"
    };

    for (int d = 0; d < 2; ++d) {

        std::vector<std::string> frames;

        for (int i = 1; i <= 4; ++i) {

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

        std::string pref = std::string("death_") + suffix[d];
        std::vector<std::string> frames =
            AssetPaths::collectFramesByPrefix(
                "enemies/skeleton/death",
                pref
            );

        if (frames.empty()) continue;

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

    sf::Vector2f vel =
        skeleton.getVelocity();

    std::string dir;

    // diagonales usan izquierda/derecha

    if (std::abs(vel.x) >
        std::abs(vel.y)) {

        if (vel.x > 0.f)
            dir = "right";
        else
            dir = "left";
    }
    else {

        if (vel.y > 0.f)
            dir = "down";
        else
            dir = "up";
    }

    if (skeleton.isDeathAnimPending()) {
        return "death_" + dir;
    }

    if (skeleton.getAIState() == EnemyState::Attack) {

        if (dir == "left")
            return "attack_left";

        return "attack_right";
    }

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


    if (skeleton.isDeathAnimPending() &&
        anim.isFinished()) {

        skeleton.deactivate();

        return;
    }


    anim.applyToSprite(sprite, 2.f);

    if (skeleton.isHurtAnimating()) {

        sprite.setColor(
            sf::Color(255, 120, 120)
        );
    }
    else {

        sprite.setColor(
            sf::Color::White
        );
    }

    sprite.setPosition(
        skeleton.getPosition()
    );

    skeleton.getSprite() = sprite;
}