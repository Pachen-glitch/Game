#include "SkeletonEnemy.h"

#include "../player/Player.h"
#include "../../utils/AssetPaths.h"
#include "../../utils/MathUtils.h"

SkeletonEnemy::SkeletonEnemy(sf::Vector2f pos)
    : Enemy(
        pos,
        EnemyKind::Skeleton,
        6,
        AssetPaths::getEnemySprite("skeleton", "idle", 1)
    )
{
    moveSpeed = 75.f;
    chaseSpeed = 115.f;

    aggroRadius = 260.f;
    deaggroRadius = 340.f;

    maxChaseFromSpawn = 420.f;

    contactDamage = 2.f;
    contactKnockback = 170.f;
}

void SkeletonEnemy::think(Player& player, float dt, const Map& map)
{
    Enemy::think(player, dt, map);

    // Rage mode when low HP
    if (health <= 2 && !isDead()) {

        chaseSpeed = 145.f;
        moveSpeed = 95.f;
    }
}

void SkeletonEnemy::takeHit(int damage, sf::Vector2f knockback)
{
    Enemy::takeHit(damage, knockback);

    if (!isDead()) {

        // Rally behavior
        setAIState(EnemyState::CallAllies);

        // Longer alert duration
        stateTimer.start(1.0f);
    }
}