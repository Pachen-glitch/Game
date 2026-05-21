#include "Player.h"

Player::Player(int startX, int startY)
    : Entity(
        startX,
        startY,
        64,
        64,
        "assets/sprites/player/idle/player_idle.png"
    )
{
    lives = 3;

    coins = 0;
    keys = 0;

    shieldActive = false;

    attacking = false;
    spinning = false;

    direction = DOWN;
}


// MOVEMENT
void Player::onInteract(Player& player) {

}

void Player::move(int dx, int dy) {

    x += dx;
    y += dy;
}

void Player::setDirection(Direction dir) {

    direction = dir;
}

Direction Player::getDirection() {

    return direction;
}


// EVENTS


void Player::addCoin() {

    coins++;
}

void Player::addKey() {

    keys++;
}

void Player::heal(int amount) {

    lives += amount;

    if (lives > 3)
        lives = 3;
}

void Player::damage(int amount) {

    if (shieldActive)
        return;

    lives -= amount;

    if (lives < 0)
        lives = 0;
}


// ATTACKS

void Player::swordAttack() {

    attacking = true;

    // luego:
    // crear hitbox
    // sprite attack
}

void Player::spinAttack() {

    spinning = true;

    // daño en área
}

bool Player::isAttacking() {

    return attacking;
}

bool Player::isSpinning() {

    return spinning;
}


// SHIELD


void Player::activateShield() {

    shieldActive = true;
}

void Player::deactivateShield() {

    shieldActive = false;
}

bool Player::hasShield() {

    return shieldActive;
}

// GETTERS


int Player::getLives() {

    return lives;
}

int Player::getCoins() {

    return coins;
}

int Player::getKeys() {

    return keys;
}


// UPDATE

void Player::update() {

    attacking = false;

    spinning = false;
}