#include "SlimeEnemy.h"
#include "../base/EntityManager.h"
#include "../player/Player.h"

SlimeEnemy::SlimeEnemy(sf::Vector2f pos)
    : Enemy(pos, EnemyKind::Slime, 2,
        "assets/sprites/enemies/slime/slime_idle.png")
{
    moveSpeed = 70.f;
}

void SlimeEnemy::think(Player& player, float dt) {
    Enemy::think(player, dt);
}

void SlimeEnemy::onDeathSplit(sf::Vector2f pos, EntityManager& manager) {
    manager.spawn<SlimeEnemy>(pos + sf::Vector2f(-16.f, 0.f));
    manager.spawn<SlimeEnemy>(pos + sf::Vector2f(16.f, 0.f));
}
