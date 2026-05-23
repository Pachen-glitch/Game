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
    hopCooldown.start(0.4f);
}

void SlimeEnemy::initKindStats() {
    moveSpeed = 52.f;
    chaseSpeed = 68.f;
    aggroRadius = 120.f;
    deaggroRadius = 200.f;
    aggroMemoryDuration = 2.8f;
    maxChaseFromSpawn = 180.f;
}

void SlimeEnemy::think(Player& player, float dt, const Map& map) {
    if (deathAnimPending) return;
    updateHopTimers(dt);
    Enemy::think(player, dt, map);
}

void SlimeEnemy::updateHopTimers(float dt) {
    hopCooldown.tick(dt);
    hopBurstTimer.tick(dt);

    if (aiState == EnemyState::Chase || aiState == EnemyState::Attack) {
        hopBurstActive = true;
        return;
    }

    if (hopBurstActive && hopBurstTimer.finished()) {
        hopBurstActive = false;
        hopCooldown.start(0.55f + randomRange(0.f, 0.9f));
    }

    if (!hopBurstActive && hopCooldown.finished()) {
        hopBurstActive = true;
        hopBurstTimer.start(0.14f + randomRange(0.f, 0.08f));
    }
}

void SlimeEnemy::modifyVelocity(float dt) {
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

void SlimeEnemy::takeHit(int damage, sf::Vector2f knockback) {
    Enemy::takeHit(damage, knockback);
}

void SlimeEnemy::onDeathSplit(sf::Vector2f pos, EntityManager& manager) {
    manager.spawn<SlimeEnemy>(pos + sf::Vector2f(-16.f, 0.f));
    manager.spawn<SlimeEnemy>(pos + sf::Vector2f(16.f, 0.f));
}
