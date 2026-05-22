#include "BatEnemy.h"

#include <cmath>
#include "../../core/Constants.h"
#include "../player/Player.h"
#include "../../utils/MathUtils.h"

BatEnemy::BatEnemy(sf::Vector2f pos)
    : Enemy(pos, EnemyKind::Bat, 1,
        "assets/sprites/enemies/bat/bat_idle.png")
{
    moveSpeed = Constants::BAT_SPEED;
    aggroRadius = 200.f;
}

void BatEnemy::think(Player& player, float dt) {
    Enemy::think(player, dt);
    if (getAIState() == EnemyState::Chase) {
        sf::Vector2f wobble(std::sin(stateTimer.getRemaining() * 8.f) * 20.f, 0.f);
        velocity += wobble * dt;
    }
}
