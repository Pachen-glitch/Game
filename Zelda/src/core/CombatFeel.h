#pragma once

#include <SFML/System/Vector2.hpp>

// Brief hit-pause, screen shake, and combat pacing — used by GameSession.
class CombatFeel {
public:
    static CombatFeel& instance();

    void tick(float dt);
    void triggerHitPause(float duration = 0.05f);
    void triggerScreenShake(float intensity, float duration);
    float getTimeScale() const;
    sf::Vector2f getShakeOffset() const;

    void setEnemyFlashTimer(float duration = 0.12f);
    bool isEnemyFlashing() const;
    float getFlashPhase() const;

private:
    CombatFeel() = default;
    float hitPauseRemaining = 0.f;
    float enemyFlashRemaining = 0.f;
    float shakeRemaining = 0.f;
    float shakeIntensity = 0.f;
    float shakeDuration = 0.f;
};
