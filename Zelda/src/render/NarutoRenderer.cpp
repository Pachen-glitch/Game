#include "NarutoRenderer.h"

#include "../entity/player/Direction.h"
#include "../utils/AssetPaths.h"

#include <algorithm>
#include <cmath>

namespace {

const char* kNarutoFolder = "enemies/naruto";

constexpr float kRasenCastFrameDuration = 0.18f;
constexpr float kOdamaCastFrameDuration = 0.18f;
constexpr float kRunSpeedThreshold = 12.f;
// Devuelve el clip de movimiento para el boss
std::string movementClipForBoss(const NarutoBoss& boss) {
    sf::Vector2f vel = boss.getVelocity();
    float speed = std::sqrt(vel.x * vel.x + vel.y * vel.y);
    if (speed <= kRunSpeedThreshold) {
        return boss.facesLeft() ? "idle_l" : "idle_r";
    }
    if (std::abs(vel.x) > 4.f) {
        return vel.x < 0.f ? "run_l" : "run_r";
    }
    return boss.facesLeft() ? "run_l" : "run_r";
}
// Registra un clip
void registerClip(
    AnimationManager& anim,
    const std::string& name,
    const std::vector<std::string>& frames,
    float frameDuration,
    bool loop
) {
    if (frames.empty()) return;
    anim.registerAnimation(name, AssetPaths::buildAnimation(frames, frameDuration, loop));
}
// Registra un clip con un prefijo
void registerPrefixClip(
    AnimationManager& anim,
    const std::string& clipName,
    const std::string& framePrefix,
    float frameDuration,
    bool loop
) {
    auto frames = AssetPaths::collectFramesByPrefix(kNarutoFolder, framePrefix);
    registerClip(anim, clipName, frames, frameDuration, loop);
}

void registerPrefixOrSingleClip(
    AnimationManager& anim,
    const std::string& clipName,
    const std::string& framePrefix,
    float frameDuration,
    bool loop
) {
    auto frames = AssetPaths::collectFramesByPrefix(kNarutoFolder, framePrefix);
    if (frames.empty()) {
        std::string single = AssetPaths::resolveFirst({
            std::string(kNarutoFolder) + "/" + framePrefix + ".png"
        });
        if (!single.empty()) {
            frames.push_back(single);
        }
    }
    registerClip(anim, clipName, frames, frameDuration, loop);
}

std::vector<std::string> collectNumberedFrames(
    const std::string& baseName,
    int firstIndex,
    int lastIndex
) {
    std::vector<std::string> frames;
    for (int i = firstIndex; i <= lastIndex; ++i) {
        frames.push_back(AssetPaths::resolveFirst({
            std::string(kNarutoFolder) + "/" + baseName +
            std::to_string(i) + ".png"
        }));
    }
    return frames;
}

} // namespace

NarutoRenderer::NarutoRenderer() {
    buildAnimations();
    anim.play("idle_l");
    anim.applyToSprite(sprite, 2.f);
}
// Construye las animaciones
void NarutoRenderer::buildAnimations() {
    registerPrefixClip(anim, "run_l", "running_l", 0.08f, true);
    registerPrefixClip(anim, "run_r", "running_r", 0.08f, true);
    registerPrefixClip(anim, "kick_l", "punch_l", 0.07f, false);
    registerPrefixClip(anim, "kick_r", "punch_r", 0.07f, false);
    registerPrefixClip(anim, "odama_l", "odama_l", kOdamaCastFrameDuration, false);
    registerPrefixClip(anim, "odama_r", "odama_r", kOdamaCastFrameDuration, false);
    registerPrefixClip(anim, "rasen_l", "rasenShuriken_l", kRasenCastFrameDuration, false);
    registerPrefixClip(anim, "rasen_r", "rasenShuriken_r", kRasenCastFrameDuration, false);
    registerPrefixClip(anim, "hit_l", "hit_l", 0.1f, false);
    registerPrefixClip(anim, "hit_r", "hit_r", 0.1f, false);
    registerPrefixOrSingleClip(anim, "clone_l", "invoque_l", 0.11f, false);
    registerPrefixOrSingleClip(anim, "clone_r", "invoque_r", 0.11f, false);

    {
        auto idleL = AssetPaths::collectFramesByPrefix(kNarutoFolder, "idle_l");
        if (idleL.empty()) {
            idleL.push_back(AssetPaths::resolveFirst({
                std::string(kNarutoFolder) + "/idle_l.png"
            }));
        }
        registerClip(anim, "idle_l", idleL, 0.2f, true);

        auto idleR = AssetPaths::collectFramesByPrefix(kNarutoFolder, "idle_r");
        if (idleR.empty()) {
            idleR.push_back(AssetPaths::resolveFirst({
                std::string(kNarutoFolder) + "/idle_r.png"
            }));
        }
        registerClip(anim, "idle_r", idleR, 0.2f, true);
    }

    registerClip(
        anim,
        "substitution",
        collectNumberedFrames("sustitucion_", 1, 5),
        0.1f,
        false
    );

    registerClip(
        anim,
        "boss_spawn_smoke",
        collectNumberedFrames("sustitucion_", 2, 5),
        0.14f,
        false
    );

    registerClip(// Registra un clip
        anim,
        "clone_smoke",
        collectNumberedFrames("sustitucion_", 2, 5),
        0.12f,
        false
    );

    {
        auto vanishFrames = collectNumberedFrames("sustitucion_", 2, 5);
        std::reverse(vanishFrames.begin(), vanishFrames.end());
        registerClip(anim, "clone_vanish_smoke", vanishFrames, 0.1f, false);
    }

    registerPrefixClip(anim, "death", "explosion", 0.14f, false);
}
// Devuelve el clip para el boss
std::string NarutoRenderer::clipForBoss(NarutoBoss& boss) const {
    if (boss.isDeathAnimPending()) {
        return "death";
    }
    if (boss.isIntroSmokeActive()) {
        return "boss_spawn_smoke";
    }
    if (boss.isSubstitutionReappearSmoke()) {
        return "substitution";
    }
    if (boss.isInIntroMode()) {
        return boss.facesLeft() ? "idle_l" : "idle_r";
    }
    if (boss.isHurtAnimating()) {
        return boss.facesLeft() ? "hit_l" : "hit_r";
    }

    switch (boss.getCurrentAttack()) {
        case NarutoAttack::Kick:
            return boss.facesLeft() ? "kick_l" : "kick_r";
        case NarutoAttack::Dash:
        case NarutoAttack::Run:
        case NarutoAttack::Retreat:
            return movementClipForBoss(boss);
        case NarutoAttack::OdamaRasengan:
            return boss.facesLeft() ? "odama_l" : "odama_r";
        case NarutoAttack::RasenShuriken:
            return boss.facesLeft() ? "rasen_l" : "rasen_r";
        case NarutoAttack::CloneSummon:
            return boss.facesLeft() ? "clone_l" : "clone_r";
        case NarutoAttack::Substitution:
        case NarutoAttack::DefensiveSubstitution:
            return "substitution";
        default:
            break;
    }

    sf::Vector2f vel = boss.getVelocity();
    float speed = std::sqrt(vel.x * vel.x + vel.y * vel.y);
    if (speed > kRunSpeedThreshold) {
        if (std::abs(vel.x) > 4.f) {
            return vel.x < 0.f ? "run_l" : "run_r";
        }
        return boss.facesLeft() ? "run_l" : "run_r";
    }
    return boss.facesLeft() ? "idle_l" : "idle_r";
}
// Devuelve el clip para el clone
std::string NarutoRenderer::clipForClone(NarutoCloneEnemy& clone) const {
    if (clone.isVanishing() || clone.isDeathAnimPending()) {
        return "clone_vanish_smoke";
    }
    if (clone.isSpawning()) {
        return "clone_smoke";
    }
    if (clone.isHurtAnimating()) {
        return clone.getFacingDirection() == Direction::LEFT ? "hit_l" : "hit_r";
    }
    if (clone.isAttacking() || clone.isKickActive()) {
        return clone.getFacingDirection() == Direction::LEFT ? "kick_l" : "kick_r";
    }

    sf::Vector2f vel = clone.getVelocity();
    float speed = std::sqrt(vel.x * vel.x + vel.y * vel.y);
    if (speed > 8.f) {
        return vel.x < 0.f ? "run_l" : "run_r";
    }
    return clone.getFacingDirection() == Direction::LEFT ? "idle_l" : "idle_r";
}

void NarutoRenderer::update(NarutoBoss& boss, float deltaTime) {
    const bool drawSmokeOnly =
        boss.isIntroSmokeActive() || boss.isSubstitutionReappearSmoke();
    const bool drawBoss =
        drawSmokeOnly || !boss.isHidden();

    std::string clip = clipForBoss(boss);
    NarutoAttack attack = boss.getCurrentAttack();
    bool restart = clip != lastClip || attack != lastTrackedAttack;

    if (boss.getCurrentAttack() == NarutoAttack::CloneSummon &&
        clip == lastClip && anim.isFinished()) {
        restart = true;
    }

    if (drawSmokeOnly && clip == lastClip && anim.isFinished()) {
        restart = true;
    }

    if (boss.isSubstitutionReappearSmoke() &&
        clip == "substitution" && anim.isFinished()) {
        restart = true;
    }

    if (restart) {
        anim.play(clip, true);
        lastClip = clip;
        lastTrackedAttack = attack;
    }

    anim.update(deltaTime);

    if (boss.isDeathAnimPending() && anim.isFinished()) {
        boss.deactivate();
        return;
    }

    if (!drawBoss) {
        return;
    }

    anim.applyToSprite(sprite, 2.f);

    if (drawSmokeOnly) {
        sprite.setColor(sf::Color(255, 255, 255, 230));
    } else if (boss.getChakraBurstTimer() > 0.f) {
        float t = boss.getChakraBurstTimer() / 0.75f;
        float pulse = 0.65f + 0.35f * std::sin((1.f - t) * 12.f);
        auto channel = static_cast<sf::Uint8>(180.f + pulse * 75.f);
        sprite.setColor(sf::Color(channel, channel, 255));
        float scale = 2.f + pulse * 0.18f;
        anim.applyToSprite(sprite, scale);
    } else if (boss.isHurtAnimating()) {
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
        anim.play(clip, true);
        lastClip = clip;
    }

    anim.update(deltaTime);

    if (clone.isDeathAnimPending() && anim.isFinished()) {
        clone.deactivate();
        return;
    }

    anim.applyToSprite(sprite, 2.f);
    if (clone.isVanishing() || clone.isDeathAnimPending() || clone.isSpawning()) {
        sprite.setColor(sf::Color(255, 255, 255, 230));
    } else {
        sprite.setColor(sf::Color::White);
    }
    sprite.setPosition(clone.getPosition());
    clone.getSprite() = sprite;
}

void NarutoRenderer::drawBoss(sf::RenderWindow& window, NarutoBoss& boss) {
    if (boss.isIntroSmokeActive() || !boss.isHidden()) {
        window.draw(boss.getSprite());
    }
}

void NarutoRenderer::drawClone(sf::RenderWindow& window, NarutoCloneEnemy& clone) {
    window.draw(clone.getSprite());
}
