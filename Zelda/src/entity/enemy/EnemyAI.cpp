#include "EnemyAI.h"
#include "Enemy.h"
#include "../player/Player.h"
#include "../../utils/MathUtils.h"

bool EnemyAI::playerInRadius(sf::Vector2f a, sf::Vector2f b, float radius) {
    return MathUtils::distance(a, b) <= radius;
}

void EnemyAI::updatePatrol(Enemy& enemy, float dt) {
    (void)enemy;
    (void)dt;
}

void EnemyAI::updateChase(Enemy& enemy, Player& player, float dt, float speed) {
    (void)dt;
    sf::Vector2f dir = MathUtils::directionTo(
        enemy.getPosition(), player.getPosition()
    );
    enemy.setPosition(enemy.getPosition() + dir * speed * dt);
}

void EnemyAI::faceTarget(Enemy& enemy, sf::Vector2f target) {
    (void)enemy;
    (void)target;
}
