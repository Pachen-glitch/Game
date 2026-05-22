#pragma once

// Persistent / run upgrades — separate from transient combat state.
struct PlayerStats {
    int maxHearts = 3;
    int hearts = 3;
    int rupees = 0;
    int keys = 0;

    float swordDamage = 1.f;
    float attackSpeedMult = 1.f;
    float moveSpeedMult = 1.f;

    bool hasSpinAttack = true;
    bool hasShield = true;

    void heal(int amount);
    void takeDamage(int amount);
    bool isDead() const;
};
