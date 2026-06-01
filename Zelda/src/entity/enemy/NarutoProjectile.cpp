#include "NarutoProjectile.h"

#include "../player/Player.h"
#include "../../render/TextureCache.h"
#include "../../utils/AssetPaths.h"
#include "../../utils/MathUtils.h"

#include <algorithm>
#include <cmath>

namespace {

const char* kNarutoFolder = "enemies/naruto";

constexpr float kRasenProjectileFrameDuration = 0.115f;
constexpr float kOdamaProjectileFrameDuration = 0.12f;
constexpr float kRasenVisualScaleMultiplier = 1.75f;
constexpr float kHomingDuration = 2.f;
constexpr float kMaxTurnRate = 1.55f;

float wrapAngle(float radians) {
    constexpr float kPi = 3.14159265f;
    constexpr float kTwoPi = kPi * 2.f;
    while (radians > kPi) radians -= kTwoPi;
    while (radians < -kPi) radians += kTwoPi;
    return radians;
}

std::vector<std::string> collectProjectileFrames(NarutoProjectileType type) {
    if (type == NarutoProjectileType::Odama) {
        return AssetPaths::collectFramesByPrefix(kNarutoFolder, "odama_br");
    }
    return AssetPaths::collectFramesByPrefix(kNarutoFolder, "rasenShuriken_b");
}

} // namespace

NarutoProjectile::NarutoProjectile(
    sf::Vector2f pos,
    sf::Vector2f vel,
    NarutoProjectileType type,
    float dmg
)
    : Entity(pos, {48.f, 48.f}, "", EntityType::Projectile)
    , velocity(vel)
    , projType(type)
    , damage(dmg)
    , speed(MathUtils::length(vel))
{
    if (type == NarutoProjectileType::Odama) {
        size = {72.f, 72.f};
        lifetime = 5.f;
        knockback = 180.f;
    } else {
        size = {56.f, 56.f};
        lifetime = 3.5f;
        knockback = 200.f;
    }

    setupAnimation(type);
}

void NarutoProjectile::setupAnimation(NarutoProjectileType type) {
    auto frames = collectProjectileFrames(type);
    if (frames.empty()) return;

    const float frameDuration =
        (type == NarutoProjectileType::Odama)
            ? kOdamaProjectileFrameDuration
            : kRasenProjectileFrameDuration;

    anim.registerAnimation(
        "fly",
        AssetPaths::buildAnimation(frames, frameDuration, true)
    );
    anim.play("fly");

    const sf::Texture& tex = TextureCache::instance().get(frames.front());
    renderScale = size.x / static_cast<float>(tex.getSize().x);
    if (type == NarutoProjectileType::RasenShuriken) {
        renderScale *= kRasenVisualScaleMultiplier;
    }
    anim.applyToSprite(sprite, renderScale);
    sprite.setPosition(getPosition());
}

void NarutoProjectile::applyHomingSteer(const sf::Vector2f& targetCenter, float dt) {
    if (age >= kHomingDuration || speed < 1.f) return;

    sf::Vector2f origin = getPosition() + size * 0.5f;
    sf::Vector2f toTarget = MathUtils::directionTo(origin, targetCenter);

    float currentAngle = std::atan2(velocity.y, velocity.x);
    float targetAngle = std::atan2(toTarget.y, toTarget.x);
    float turn = wrapAngle(targetAngle - currentAngle);
    turn = std::clamp(turn, -kMaxTurnRate * dt, kMaxTurnRate * dt);

    float newAngle = currentAngle + turn;
    velocity = {std::cos(newAngle) * speed, std::sin(newAngle) * speed};
}

void NarutoProjectile::update(float dt) {
    age += dt;
    if (age >= lifetime) {
        deactivate();
        return;
    }

    setPosition(getPosition() + velocity * dt);
    anim.update(dt);
    anim.applyToSprite(sprite, renderScale);
    sprite.setPosition(getPosition());
}

void NarutoProjectile::onInteract(Player&) {}
