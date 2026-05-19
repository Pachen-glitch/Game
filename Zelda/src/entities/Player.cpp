#include "Player.h"

Player::Player(int startX, int startY) {
    x = startX;
    y = startY;
    lives = 3; // Default number of lives
}

void Player::move(int dx, int dy) {
    x += dx;
    y += dy;
}