#include "PlayerStats.h"

void PlayerStats::heal(int amount) {
    hearts += amount;
    if (hearts > maxHearts) hearts = maxHearts;
}

void PlayerStats::takeDamage(int amount) {
    hearts -= amount;
    if (hearts < 0) hearts = 0;
}

bool PlayerStats::isDead() const {
    return hearts <= 0;
}
