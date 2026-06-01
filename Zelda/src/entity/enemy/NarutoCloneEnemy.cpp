#include "NarutoCloneEnemy.h"

#include "../player/Player.h"
#include "../../utils/AssetPaths.h"
#include "../../utils/MathUtils.h"

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
    moveSpeed = 105.f;
    chaseSpeed = 115.f;
    contactDamage = 0.5f;
    contactKnockback = 90.f;
    aggroRadius = 320.f;
    deaggroRadius = 400.f;
    maxChaseFromSpawn = 9999.f;
}

void NarutoCloneEnemy::update(float dt) {
    lifeTimer -= dt;
    if (spawnTimer > 0.f) {
        spawnTimer -= dt;
        velocity = {0.f, 0.f};
    } else if (lifeTimer <= 0.f) {
        deactivate();
        return;
    }

    Enemy::update(dt);
}

float NarutoCloneEnemy::getSpawnProgress() const {
    if (spawnTimer <= 0.f) return 1.f;
    return 1.f - (spawnTimer / 0.55f);
}

void NarutoCloneEnemy::think(Player& player, float dt, const Map& map) {
    if (spawnTimer > 0.f || isDead() || deathAnimPending) {
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
