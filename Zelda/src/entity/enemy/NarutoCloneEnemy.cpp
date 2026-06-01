#include "NarutoCloneEnemy.h"

#include "../player/Player.h"
#include "../../core/CombatFeel.h"
#include "../../interaction/EventBus.h"
#include "../../utils/AssetPaths.h"
#include "../../utils/MathUtils.h"

namespace {

constexpr float kCloneVanishDuration = 0.4f;

} // namespace

NarutoCloneEnemy::NarutoCloneEnemy(sf::Vector2f pos) // Constructor de NarutoCloneEnemy
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
    moveSpeed = 105.f;
    chaseSpeed = 115.f;
    contactDamage = 0.5f;
    contactKnockback = 90.f;
    aggroRadius = 320.f;
    deaggroRadius = 400.f;
    maxChaseFromSpawn = 9999.f;
}

void NarutoCloneEnemy::startVanish() { // Activa el vanish
    if (vanishTimer > 0.f || deathAnimPending) return;

    setAIState(EnemyState::Dead);
    deathAnimPending = true;
    vanishTimer = kCloneVanishDuration;
    velocity = {0.f, 0.f};
    EventBus::instance().emit("enemy_died");
}

void NarutoCloneEnemy::takeHit(int damage, sf::Vector2f knockback) { // Recibe un hit
    if (isDead() || deathAnimPending || vanishTimer > 0.f) return;

    health -= damage;
    velocity = knockback * 1.35f;

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

void NarutoCloneEnemy::update(float dt) { // Actualiza el enemigo
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

float NarutoCloneEnemy::getSpawnProgress() const { // Devuelve el progreso de spawn
    if (spawnTimer <= 0.f) return 1.f;
    return 1.f - (spawnTimer / 0.72f);
}

void NarutoCloneEnemy::think(Player& player, float dt, const Map& map) { // Piensa el enemigo
    if (spawnTimer > 0.f || vanishTimer > 0.f || isDead() || deathAnimPending) {
        return;
    }

    if (hurtAnimTimer.isActive()) {
        setAIState(EnemyState::Hurt);
        applyMovement(dt, map);
        return;
    }

    setAIState(EnemyState::Chase);
    updateChase(player, dt);
    applyMovement(dt, map);
}
