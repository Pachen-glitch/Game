#include "Player.h"

Player::Player(int startX, int startY) {

    x = startX;
    y = startY;

    coins = 0;
    keys = 0;
    lives = 3;
}

void Player::move(int dx, int dy) {

    x += dx;
    y += dy;
}