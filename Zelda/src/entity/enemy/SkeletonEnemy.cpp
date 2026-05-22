#include "SkeletonEnemy.h"
#include "../player/Player.h"

SkeletonEnemy::SkeletonEnemy(sf::Vector2f pos)
    : Enemy(pos, EnemyKind::Skeleton, 4,
        "assets/sprites/enemies/skeleton/skeleton_idle.png")
{
    moveSpeed = 80.f;
}

void SkeletonEnemy::think(Player& player, float dt) {
    Enemy::think(player, dt);
}

void SkeletonEnemy::takeHit(int damage, sf::Vector2f knockback) {
    Enemy::takeHit(damage, knockback);
    if (!isDead()) {
        setAIState(EnemyState::CallAllies);
        stateTimer.start(0.5f);
    }
}
