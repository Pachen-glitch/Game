#include "CombatFeel.h"

CombatFeel& CombatFeel::instance() {
    static CombatFeel feel;
    return feel;
}

void CombatFeel::triggerHitPause(float duration) {
    if (duration > hitPauseRemaining) {
        hitPauseRemaining = duration;
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
}

float CombatFeel::getTimeScale() const {
    return hitPauseRemaining > 0.f ? 0.08f : 1.f;
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
