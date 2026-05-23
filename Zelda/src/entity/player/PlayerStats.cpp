#include "PlayerStats.h"

#include <algorithm>
#include <cmath>

void PlayerStats::heal(float amount) {
    hearts += amount;
    if (hearts > maxHearts) hearts = maxHearts;
}

void PlayerStats::takeDamage(float amount) {
    hearts -= amount;
    if (hearts < 0.f) hearts = 0.f;
}

bool PlayerStats::isDead() const {
    return hearts <= 0.f;
}

int PlayerStats::getHeartContainers() const {
    return static_cast<int>(std::ceil(maxHearts));
}

int PlayerStats::getDisplayHearts() const {
    return static_cast<int>(std::ceil(hearts));
}
