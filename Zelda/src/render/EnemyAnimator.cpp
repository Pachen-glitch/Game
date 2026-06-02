#include "EnemyAnimator.h"
#include "../core/CombatFeel.h"
#include "../utils/AssetPaths.h"

#include <cmath>

void EnemyAnimator::setupSlime() {

    auto reg = [this](const std::string& name,
                      const std::string& state,
                      float dur,
                      bool loop) {

        auto frames =// Colecciona los frames por prefijo
            AssetPaths::getEnemyAnimFrames("slime", state);

        Animation clip =// Construye la animacion
            AssetPaths::buildAnimation(frames, dur, loop);
// Registra la animacion
        if (!clip.getCurrentFramePath().empty()) {
            anim.registerAnimation(name, clip);
        }
    };

    reg("idle", "idle", 0.14f, true);// Idle
    reg("walk", "walk", 0.10f, true);
    reg("hurt", "hurt", 0.12f, false);
    reg("death", "death", 0.14f, false);

    lastClip.clear();
    deathFinished = false;
}

void EnemyAnimator::setupSkeleton() {

    auto reg = [this](const std::string& name,
                      const std::string& state,
                      float dur,
                      bool loop) {

        auto frames =// Colecciona los frames por prefijo
            AssetPaths::getEnemyAnimFrames("skeleton", state);

        Animation clip =// Construye la animacion
            AssetPaths::buildAnimation(frames, dur, loop);

        if (!clip.getCurrentFramePath().empty()) {
            anim.registerAnimation(name, clip);
        }
    };

    reg("idle", "idle", 0.14f, true);
    reg("walk", "walk", 0.10f, true);
    reg("hurt", "hurt", 0.12f, false);
    reg("death", "death", 0.14f, false);

    lastClip.clear();
    deathFinished = false;
}

void EnemyAnimator::setupSummoner() {

    auto reg = [this](const std::string& name,
                      const std::string& state,
                      float dur,
                      bool loop) {

        auto frames =
            AssetPaths::getEnemyAnimFrames("summoner", state);// Colecciona los frames por prefijo

        Animation clip =// Construye la animacion
            AssetPaths::buildAnimation(frames, dur, loop);

        if (!clip.getCurrentFramePath().empty()) {
            anim.registerAnimation(name, clip);
        }
    };

    reg("idle", "idle", 0.14f, true);
    reg("walk", "walk", 0.10f, true);
    reg("hurt", "hurt", 0.12f, false);
    reg("death", "death", 0.14f, false);

    lastClip.clear();
    deathFinished = false;
}

void EnemyAnimator::setupBat() {

    auto reg = [this](const std::string& name,
                      const std::string& state,
                      float dur,
                      bool loop) {

        auto frames =
            AssetPaths::getEnemyAnimFrames("bat", state);// Colecciona los frames por prefijo

        Animation clip =// Construye la animacion
            AssetPaths::buildAnimation(frames, dur, loop);

        if (!clip.getCurrentFramePath().empty()) {
            anim.registerAnimation(name, clip);
        }
    };

    reg("walk", "walk", 0.10f, true);
    reg("hurt", "hurt", 0.12f, false);
    reg("death", "death", 0.14f, false);

    lastClip.clear();
    deathFinished = false;
}

void EnemyAnimator::update(Enemy& enemy, float dt) {

    if (enemy.isDeathAnimPending()) {

        if (lastClip != "death") {// Si el ultimo clip no es death

            anim.play("death", true);

            lastClip = "death";
        }

        anim.update(dt);

        // Si no existe animación de muerte,
        // o terminó la animación,
        // eliminar enemigo inmediatamente.

        if (anim.isFinished()) {

            deathFinished = true;

            enemy.deactivate();
        }

        return;
    }

    std::string clip =
        pickClip(enemy);

    if (clip != lastClip) {

        anim.play(clip);

        lastClip = clip;
    }

    anim.update(dt);
}

std::string EnemyAnimator::pickClip(
    const Enemy& enemy
) const {

    if (enemy.isDeathAnimPending()) {
        return "death";
    }

    if (enemy.isHurtAnimating() ||
        enemy.getAIState() == EnemyState::Hurt) {

        return "hurt";
    }

    EnemyState st =
        enemy.getAIState();

    if (st == EnemyState::Chase ||
        st == EnemyState::Wander ||
        st == EnemyState::ReturnToSpawn) {

        sf::Vector2f vel =
            enemy.getVelocity();

        float speed =
            std::sqrt(
                vel.x * vel.x +
                vel.y * vel.y
            );

        if (speed > 6.f) {
            return "walk";
        }
    }

    if (st == EnemyState::Attack) {

        sf::Vector2f vel =
            enemy.getVelocity();

        float speed =
            std::sqrt(
                vel.x * vel.x +
                vel.y * vel.y
            );

        if (speed > 6.f) {
            return "walk";
        }
    }

    return "idle";
}

void EnemyAnimator::applyToEntity(
    Entity& entity,
    float scale
) {

    anim.applyToSprite(
        entity.getSprite(),
        scale
    );

    auto* enemy =
        dynamic_cast<Enemy*>(&entity);

    if (enemy &&
        (enemy->isHurtAnimating() ||
         enemy->getAIState() == EnemyState::Hurt)) {

        entity.getSprite().setColor(
            sf::Color(255, 140, 140)
        );
    }
    else {

        entity.getSprite().setColor(
            sf::Color::White
        );
    }

    entity.getSprite().setPosition(
        entity.getPosition()
    );
}