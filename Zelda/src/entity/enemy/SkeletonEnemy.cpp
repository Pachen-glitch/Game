#include "SkeletonEnemy.h"
#include "../player/Player.h"
#include "../../utils/AssetPaths.h"

SkeletonEnemy::SkeletonEnemy(sf::Vector2f pos)
    : Enemy(pos, EnemyKind::Skeleton, 4, AssetPaths::getEnemySprite("skeleton"))
{
    moveSpeed = 80.f;
}

void SkeletonEnemy::think(Player& player, float dt, const Map& map) {
    Enemy::think(player, dt, map);
}

void SkeletonEnemy::takeHit(int damage, sf::Vector2f knockback) {
    Enemy::takeHit(damage, knockback);
    if (!isDead()) {
        setAIState(EnemyState::CallAllies);
        stateTimer.start(0.5f);
    }
}
