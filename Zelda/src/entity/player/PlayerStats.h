#pragma once

struct PlayerStats {
    float maxHearts = 7.f;
    float hearts = 7.f;
    int rupees = 0;
    int keys = 0;

    float swordDamage = 1.f;
    float attackSpeedMult = 1.f;
    float moveSpeedMult = 1.f;

    bool hasSpinAttack = true;
    bool hasShield = true;

    void heal(float amount);
    void takeDamage(float amount);
    bool isDead() const;

    int getHeartContainers() const;
    int getDisplayHearts() const;
};
