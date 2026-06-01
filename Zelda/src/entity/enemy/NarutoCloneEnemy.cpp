#include "NarutoCloneEnemy.h"

#include "../player/Player.h"
#include "../../core/CombatFeel.h"
#include "../../interaction/EventBus.h"
#include "../../utils/AssetPaths.h"
#include "../../utils/MathUtils.h"

#include <algorithm>

namespace {

constexpr float kCloneVanishDuration = 0.4f;
constexpr float kKickDuration = 0.55f;
constexpr float kKickTriggerDist = 52.f;
constexpr float kKickReach = 44.f;
constexpr float kKickBodyOverlap = 16.f;
constexpr float kKickRetryCooldown = 0.4f;
constexpr float kCloneMeleeDamage = 0.75f;
constexpr float kNarutoMoveSpeed = 95.f;
constexpr float kNarutoChaseSpeed = 118.f;
constexpr float kCloneSpeedMultiplier = 1.3f;

} // namespace

NarutoCloneEnemy::NarutoCloneEnemy(sf::Vector2f pos)
    : Enemy(
        pos,
        EnemyKind::NarutoClone,
        1,
        AssetPaths::resolveFirst({
            "sprites/enemies/naruto/spawn_1.png",
            "sprites/enemies/naruto/idle_l.png"
        })
    )
{
    moveSpeed = kNarutoMoveSpeed * kCloneSpeedMultiplier;
    chaseSpeed = kNarutoChaseSpeed * kCloneSpeedMultiplier;
    contactDamage = kCloneMeleeDamage;
    contactKnockback = 100.f;
    aggroRadius = 9999.f;
    deaggroRadius = 9999.f;
    maxChaseFromSpawn = 9999.f;
}

void NarutoCloneEnemy::startVanish() {
    if (vanishTimer > 0.f || deathAnimPending) return;

    kickActive = false;
    attackTimer = 0.f;
    setAIState(EnemyState::Dead);
    deathAnimPending = true;
    vanishTimer = kCloneVanishDuration;
    velocity = {0.f, 0.f};
    EventBus::instance().emit("enemy_died");
}

void NarutoCloneEnemy::startKick() {
    attackTimer = 0.f;
    kickActive = false;
    setAIState(EnemyState::Attack);
}

void NarutoCloneEnemy::updateKick(const Player& player, float dt, const Map& map) {
    attackTimer += dt;

    sf::Vector2f toPlayer = MathUtils::directionTo(
        getPosition(),
        player.getPosition()
    );

    if (attackTimer < 0.10f) {
        velocity = toPlayer * (moveSpeed * 1.25f);
    } else if (attackTimer < 0.44f) {
        if (attackTimer < 0.18f) {
            velocity = toPlayer * (moveSpeed * 0.55f);
        } else {
            velocity = {0.f, 0.f};
        }
        if (!kickActive) {
            kickActive = true;
            contactCooldown.reset();
        }
    } else {
        kickActive = false;
        velocity = {0.f, 0.f};
        if (attackTimer >= kKickDuration) {
            finishKick();
        }
    }

    applyMovement(dt, map);
}

void NarutoCloneEnemy::finishKick() {
    attackTimer = 0.f;
    kickActive = false;
    kickCooldown = kKickRetryCooldown;
    setAIState(EnemyState::Chase);
    resetContactCooldown(0.85f);
}

void NarutoCloneEnemy::takeHit(int damage, sf::Vector2f knockback) {
    if (isDead() || deathAnimPending || vanishTimer > 0.f) return;

    health -= damage;
    velocity = knockback * 1.35f;
    kickActive = false;
    attackTimer = 0.f;

    CombatFeel::instance().triggerHitPause(0.05f);
    CombatFeel::instance().setEnemyFlashTimer(0.14f);
    EventBus::instance().emit("enemy_hit");

    if (health <= 0) {
        startVanish();
    } else {
        setAIState(EnemyState::Hurt);
        hurtAnimTimer.start(0.36f);
    }
}

void NarutoCloneEnemy::update(float dt) {
    if (vanishTimer > 0.f) {
        vanishTimer -= dt;
        velocity = {0.f, 0.f};
        return;
    }

    lifeTimer -= dt;
    if (spawnTimer > 0.f) {
        spawnTimer -= dt;
        velocity = {0.f, 0.f};
    } else if (lifeTimer <= 0.f) {
        startVanish();
        return;
    }

    Enemy::update(dt);
}

float NarutoCloneEnemy::getSpawnProgress() const {
    if (spawnTimer <= 0.f) return 1.f;
    return 1.f - (spawnTimer / 0.72f);
}

sf::FloatRect NarutoCloneEnemy::getContactBounds() const {
    if (vanishTimer > 0.f || deathAnimPending || spawnTimer > 0.f || !kickActive) {
        return sf::FloatRect(0.f, 0.f, 0.f, 0.f);
    }

    sf::Vector2f pos = getPosition();
    sf::Vector2f sz = getSize();
    const bool faceLeft = getFacingDirection() == Direction::LEFT;
    const float y = pos.y + 6.f;
    const float h = sz.y - 12.f;

    if (faceLeft) {
        return sf::FloatRect(
            pos.x - kKickReach,
            y,
            kKickReach + kKickBodyOverlap,
            h
        );
    }

    return sf::FloatRect(
        pos.x + sz.x - kKickBodyOverlap,
        y,
        kKickReach + kKickBodyOverlap,
        h
    );
}

void NarutoCloneEnemy::think(Player& player, float dt, const Map& map) {
    if (spawnTimer > 0.f || vanishTimer > 0.f || isDead() || deathAnimPending) {
        return;
    }

    if (hurtAnimTimer.isActive()) {
        setAIState(EnemyState::Hurt);
        applyMovement(dt, map);
        return;
    }

    if (attackTimer > 0.f) {
        updateKick(player, dt, map);
        return;
    }

    kickCooldown = std::max(0.f, kickCooldown - dt);

    setAIState(EnemyState::Chase);
    updateChase(player, dt);

    const float dist = MathUtils::distance(getPosition(), player.getPosition());
    if (kickCooldown <= 0.f && dist <= kKickTriggerDist) {
        startKick();
        updateKick(player, dt, map);
        return;
    }

    applyMovement(dt, map);
}
