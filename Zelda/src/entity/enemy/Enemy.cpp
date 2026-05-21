#include "Enemy.h"
#include "../player/Player.h"

Enemy::Enemy(int startX, int startY)
    : Entity(
        startX,
        startY,
        64,
        64,
        "assets/sprites/enemies/slime/slime_idle.png"
    )
{
    health = 2;
}

void Enemy::damage(int amount) {

    health -= amount;

    if (health <= 0) {

        deactivate();
    }
}

bool Enemy::isDead() {

    return !active;
}

void Enemy::onInteract(Player& player) {

    player.damage(1);
}

void Enemy::update() {

}