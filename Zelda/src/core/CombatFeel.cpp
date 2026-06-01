#include "CombatFeel.h"

#include <algorithm>
#include <cstdlib>
#include <cmath>

CombatFeel& CombatFeel::instance() {
    static CombatFeel feel;
    return feel;
}

void CombatFeel::triggerHitPause(float duration) {
    if (duration > hitPauseRemaining) {
        hitPauseRemaining = duration;
    }
}

void CombatFeel::triggerScreenShake(float intensity, float duration) {
    if (duration > shakeRemaining) {
        shakeIntensity = intensity;
        shakeRemaining = duration;
        shakeDuration = duration;
    }
}

void CombatFeel::tick(float dt) {
    if (hitPauseRemaining > 0.f) {
        hitPauseRemaining -= dt;
        if (hitPauseRemaining < 0.f) hitPauseRemaining = 0.f;
    }
    if (enemyFlashRemaining > 0.f) {
        enemyFlashRemaining -= dt;
        if (enemyFlashRemaining < 0.f) enemyFlashRemaining = 0.f;
    }
    if (shakeRemaining > 0.f) {
        shakeRemaining -= dt;
        if (shakeRemaining < 0.f) shakeRemaining = 0.f;
    }
}

float CombatFeel::getTimeScale() const {
    return hitPauseRemaining > 0.f ? 0.08f : 1.f;
}

sf::Vector2f CombatFeel::getShakeOffset() const {
    if (shakeRemaining <= 0.f) return {0.f, 0.f};

    float falloff = shakeDuration > 0.f ? shakeRemaining / shakeDuration : 0.f;
    float amp = shakeIntensity * falloff;
    float rx = (static_cast<float>(std::rand()) / RAND_MAX * 2.f - 1.f) * amp;
    float ry = (static_cast<float>(std::rand()) / RAND_MAX * 2.f - 1.f) * amp;
    return {rx, ry};
}

void CombatFeel::setEnemyFlashTimer(float duration) {
    enemyFlashRemaining = duration;
}

bool CombatFeel::isEnemyFlashing() const {
    return enemyFlashRemaining > 0.f;
}

float CombatFeel::getFlashPhase() const {
    return enemyFlashRemaining;
}
