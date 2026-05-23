#pragma once

// Brief hit-pause and combat pacing — used by GameSession.
class CombatFeel {
public:
    static CombatFeel& instance();

    void tick(float dt);
    void triggerHitPause(float duration = 0.05f);
    float getTimeScale() const;

    void setEnemyFlashTimer(float duration = 0.12f);
    bool isEnemyFlashing() const;
    float getFlashPhase() const;

private:
    CombatFeel() = default;
    float hitPauseRemaining = 0.f;
    float enemyFlashRemaining = 0.f;
};
