#include "CombatFeel.h"

#include <algorithm>
#include <cstdlib>
#include <cmath>

CombatFeel& CombatFeel::instance() { // Devuelve la instancia de CombatFeel
    static CombatFeel feel;
    return feel;
}

void CombatFeel::triggerHitPause(float duration) { // Activa el hit pause
    if (duration > hitPauseRemaining) {
        hitPauseRemaining = duration;
    }
}

void CombatFeel::triggerScreenShake(float intensity, float duration) { // Activa el shake screen
    if (duration > shakeRemaining) {
        shakeIntensity = intensity;
        shakeRemaining = duration;
        shakeDuration = duration;
    }
}

void CombatFeel::tick(float dt) { // Actualiza el tiempo de hit pause, shake screen y flash del enemigo
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

float CombatFeel::getTimeScale() const { // Devuelve el tiempo de escala
    return hitPauseRemaining > 0.f ? 0.08f : 1.f;
}

sf::Vector2f CombatFeel::getShakeOffset() const { // Devuelve el offset del shake screen
    if (shakeRemaining <= 0.f) return {0.f, 0.f};

    float falloff = shakeDuration > 0.f ? shakeRemaining / shakeDuration : 0.f;
    float amp = shakeIntensity * falloff;
    float rx = (static_cast<float>(std::rand()) / RAND_MAX * 2.f - 1.f) * amp;
    float ry = (static_cast<float>(std::rand()) / RAND_MAX * 2.f - 1.f) * amp;
    return {rx, ry};
}

void CombatFeel::setEnemyFlashTimer(float duration) { // Establece el tiempo de flash del enemigo
    enemyFlashRemaining = duration;
}

bool CombatFeel::isEnemyFlashing() const { // Devuelve si el enemigo esta parpadeando
    return enemyFlashRemaining > 0.f;
}

float CombatFeel::getFlashPhase() const { // Devuelve el tiempo de flash del enemigo
    return enemyFlashRemaining;
}
