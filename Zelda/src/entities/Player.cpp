#include "Player.h"

Player::Player(int startX, int startY) {
    x = startX;
    y = startY;
}

void Player::move(int dx, int dy) {
    x += dx;
    y += dy;
}