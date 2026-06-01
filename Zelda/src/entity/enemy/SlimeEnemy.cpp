#include "SlimeEnemy.h"

#include "../base/EntityManager.h"
#include "../player/Player.h"
#include "../../utils/AssetPaths.h"

SlimeEnemy::SlimeEnemy(sf::Vector2f pos)
    : Enemy(
        pos,
        EnemyKind::Slime,
        2,
        AssetPaths::getEnemySprite("slime", "idle", 1)
    )
{
    initKindStats();
    contactDamage = 0.5f;
    contactKnockback = 150.f;
    contactCooldown.start(0.f);
    hopCooldown.start(0.15f);
    stateTimer.start(randomRange(0.12f, 0.30f));
}

void SlimeEnemy::initKindStats() { // Inicializa las estadisticas del enemigo
    moveSpeed = 58.f;
    chaseSpeed = 78.f;
    aggroRadius = 200.f;
    deaggroRadius = 280.f;
    aggroMemoryDuration = 3.2f;
    maxChaseFromSpawn = 220.f;
}

void SlimeEnemy::think(Player& player, float dt, const Map& map) { // Piensa el enemigo
    if (deathAnimPending) return;
    updateHopTimers(dt);

    if (aiState == EnemyState::Idle && stateTimer.isActive()) { // Actualiza el timer de estado
        stateTimer.tick(dt * 0.85f);
    } else if (aiState == EnemyState::Wander && stateTimer.isActive()) {
        stateTimer.tick(dt * 0.65f);
    }

    Enemy::think(player, dt, map);
}

void SlimeEnemy::updateHopTimers(float dt) { // Actualiza los timers de hop
    hopCooldown.tick(dt);
    hopBurstTimer.tick(dt);

    if (aiState == EnemyState::Chase || aiState == EnemyState::Attack) {
        hopBurstActive = true;
        return;
    }

    if (hopBurstActive && hopBurstTimer.finished()) {
        hopBurstActive = false;
        hopCooldown.start(0.28f + randomRange(0.f, 0.35f));
    }

    if (!hopBurstActive && hopCooldown.finished()) {
        hopBurstActive = true;
        hopBurstTimer.start(0.20f + randomRange(0.f, 0.10f));
    }
}

void SlimeEnemy::modifyVelocity(float dt) { // Modifica la velocidad del enemigo
    (void)dt;
    if (aiState == EnemyState::Idle) {
        velocity = {0.f, 0.f};
        return;
    }

    if (aiState == EnemyState::Wander ||
        aiState == EnemyState::ReturnToSpawn) {
        if (!hopBurstActive) {
            velocity = {0.f, 0.f};
        }
        return;
    }

    if (aiState == EnemyState::Chase && !hopBurstActive) {
        velocity = {0.f, 0.f};
    }
}

void SlimeEnemy::takeHit(int damage, sf::Vector2f knockback) { // Recibe un hit
    Enemy::takeHit(damage, knockback);
}

void SlimeEnemy::onDeathSplit(sf::Vector2f pos, EntityManager& manager) { // Spawna un slime cuando el enemigo muere
    manager.spawn<SlimeEnemy>(pos + sf::Vector2f(-16.f, 0.f));
    manager.spawn<SlimeEnemy>(pos + sf::Vector2f(16.f, 0.f));
}
