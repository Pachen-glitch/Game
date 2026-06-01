#include "NarutoRenderer.h"

#include "../utils/AssetPaths.h"

#include <cmath>

namespace {

const char* kNarutoFolder = "sprites/enemies/naruto";

} // namespace

NarutoRenderer::NarutoRenderer() {
    buildAnimations();
    anim.play("idle_left");
    anim.applyToSprite(sprite, 2.f);
}

void NarutoRenderer::registerDirectionalClip(
    const std::string& name,
    const std::string& prefix,
    int frameCount,
    float frameDuration,
    bool loop
) {
    for (const char* side : {"l", "r"}) {
        std::vector<std::string> frames;
        for (int i = 1; i <= frameCount; ++i) {
            frames.push_back(AssetPaths::resolveFirst({
                std::string(kNarutoFolder) + "/" + prefix + "_" + side +
                std::to_string(i) + ".png"
            }));
        }
        Animation clip = AssetPaths::buildAnimation(frames, frameDuration, loop);
        anim.registerAnimation(name + "_" + side, clip);
    }
}

void NarutoRenderer::buildAnimations() {
    for (const char* side : {"l", "r"}) {
        std::vector<std::string> idleFrames = {
            AssetPaths::resolveFirst({
                std::string(kNarutoFolder) + "/idle_" + side + ".png"
            })
        };
        anim.registerAnimation(
            std::string("idle_") + side,
            AssetPaths::buildAnimation(idleFrames, 0.2f, true)
        );
    }

    registerDirectionalClip("run", "running", 8, 0.08f, true);
    registerDirectionalClip("kick", "punch", 5, 0.07f, false);
    registerDirectionalClip("odama", "odama", 7, 0.09f, false);
    registerDirectionalClip("rasen", "rasenShuriken", 4, 0.08f, false);
    registerDirectionalClip("clone", "invoque", 4, 0.1f, false);
    registerDirectionalClip("hit", "hit", 2, 0.1f, false);

    {
        std::vector<std::string> frames;
        for (int i = 1; i <= 5; ++i) {
            frames.push_back(AssetPaths::resolveFirst({
                std::string(kNarutoFolder) + "/sustitucion_" + std::to_string(i) + ".png"
            }));
        }
        anim.registerAnimation(
            "substitution",
            AssetPaths::buildAnimation(frames, 0.1f, false)
        );
    }

    {
        std::vector<std::string> frames;
        for (int i = 1; i <= 2; ++i) {
            frames.push_back(AssetPaths::resolveFirst({
                std::string(kNarutoFolder) + "/spawn_" + std::to_string(i) + ".png"
            }));
        }
        anim.registerAnimation(
            "spawn",
            AssetPaths::buildAnimation(frames, 0.12f, false)
        );
    }

    {
        std::vector<std::string> frames;
        for (int i = 1; i <= 2; ++i) {
            frames.push_back(AssetPaths::resolveFirst({
                std::string(kNarutoFolder) + "/explosion_" + std::to_string(i) + ".png"
            }));
        }
        anim.registerAnimation(
            "death",
            AssetPaths::buildAnimation(frames, 0.14f, false)
        );
    }
}

std::string NarutoRenderer::clipForBoss(NarutoBoss& boss) const {
    if (boss.isDeathAnimPending()) {
        return "death";
    }
    if (boss.isIntroActive()) {
        return "spawn";
    }
    if (boss.isHurtAnimating()) {
        return boss.facesLeft() ? "hit_l" : "hit_r";
    }

    switch (boss.getCurrentAttack()) {
        case NarutoAttack::Kick:
            return boss.facesLeft() ? "kick_l" : "kick_r";
        case NarutoAttack::OdamaRasengan:
            return boss.facesLeft() ? "odama_l" : "odama_r";
        case NarutoAttack::RasenShuriken:
            return boss.facesLeft() ? "rasen_l" : "rasen_r";
        case NarutoAttack::CloneSummon:
            return boss.facesLeft() ? "clone_l" : "clone_r";
        case NarutoAttack::Substitution:
            return "substitution";
        default:
            break;
    }

    sf::Vector2f vel = boss.getVelocity();
    float speed = std::sqrt(vel.x * vel.x + vel.y * vel.y);
    if (speed > 8.f) {
        return boss.facesLeft() ? "run_l" : "run_r";
    }
    return boss.facesLeft() ? "idle_l" : "idle_r";
}

std::string NarutoRenderer::clipForClone(NarutoCloneEnemy& clone) const {
    if (clone.isDeathAnimPending()) {
        return "death";
    }
    if (clone.isSpawning()) {
        return "spawn";
    }
    if (clone.isHurtAnimating()) {
        return "hit_l";
    }

    sf::Vector2f vel = clone.getVelocity();
    float speed = std::sqrt(vel.x * vel.x + vel.y * vel.y);
    if (speed > 8.f) {
        return vel.x < 0.f ? "run_l" : "run_r";
    }
    return "idle_l";
}

void NarutoRenderer::update(NarutoBoss& boss, float deltaTime) {
    if (boss.isHidden()) {
        return;
    }

    std::string clip = clipForBoss(boss);
    bool restart = clip != lastClip;
    if (restart) {
        anim.play(clip);
        lastClip = clip;
    }

    anim.update(deltaTime);

    if (boss.isDeathAnimPending() && anim.isFinished()) {
        boss.deactivate();
        return;
    }

    anim.applyToSprite(sprite, 2.f);

    if (boss.isHurtAnimating()) {
        sprite.setColor(sf::Color(255, 120, 120));
    } else {
        sprite.setColor(sf::Color::White);
    }

    sprite.setPosition(boss.getPosition());
    boss.getSprite() = sprite;
}

void NarutoRenderer::updateClone(NarutoCloneEnemy& clone, float deltaTime) {
    std::string clip = clipForClone(clone);
    bool restart = clip != lastClip;
    if (restart) {
        anim.play(clip);
        lastClip = clip;
    }

    anim.update(deltaTime);

    if (clone.isDeathAnimPending() && anim.isFinished()) {
        clone.deactivate();
        return;
    }

    anim.applyToSprite(sprite, 2.f);
    sprite.setPosition(clone.getPosition());
    clone.getSprite() = sprite;
}

void NarutoRenderer::drawBoss(sf::RenderWindow& window, NarutoBoss& boss) {
    if (!boss.isHidden()) {
        window.draw(boss.getSprite());
    }
}

void NarutoRenderer::drawClone(sf::RenderWindow& window, NarutoCloneEnemy& clone) {
    window.draw(clone.getSprite());
}
