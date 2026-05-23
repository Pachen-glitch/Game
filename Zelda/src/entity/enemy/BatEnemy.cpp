#include "BatEnemy.h"

#include <cmath>

#include "../../core/Constants.h"
#include "../player/Player.h"
#include "../../utils/AssetPaths.h"

BatEnemy::BatEnemy(sf::Vector2f pos)
    : Enemy(pos, EnemyKind::Bat, 1, AssetPaths::getEnemySprite("bat"))
{
    initKindStats();
}

void BatEnemy::initKindStats() {
    moveSpeed = 70.f;
    chaseSpeed = Constants::BAT_SPEED;
    aggroRadius = 200.f;
    deaggroRadius = 300.f;
    aggroMemoryDuration = 4.5f;
    maxChaseFromSpawn = 280.f;
}

void BatEnemy::think(Player& player, float dt, const Map& map) {
    Enemy::think(player, dt, map);
}

void BatEnemy::modifyVelocity(float dt) {
    if (aiState == EnemyState::Chase) {
        float wobble = std::sin(stateTimer.getRemaining() * 9.f) * 18.f;
        velocity.x += wobble * dt;
    }
}
