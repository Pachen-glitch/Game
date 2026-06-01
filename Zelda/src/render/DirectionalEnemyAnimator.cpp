#include "DirectionalEnemyAnimator.h"

#include "../utils/AssetPaths.h"

#include <cmath>

std::string DirectionalEnemyAnimator::directionToString(// Convierte la direccion a un string
    Direction dir
) const {

    switch (dir) { // Switch para la direccion

        case Direction::UP:
            return "arriba"; // Arriba

        case Direction::DOWN:
            return "abajo"; // Abajo

        case Direction::LEFT:
            return "izquierda"; // Izquierda

        case Direction::RIGHT:
            return "derecha"; // Derecha
    }

    return "abajo"; // Abajo
}

void DirectionalEnemyAnimator::setupSkeleton() {

    const std::string dirs[] = {
        "arriba",
        "abajo",
        "izquierda",
        "derecha"
    };

    const std::string states[] = {
        "idle",
        "walk",
        "attack",
        "death"
    };

    for (const auto& dir : dirs) {

        for (const auto& state : states) {

            std::string prefix =
                state + "_" + dir + "_";

            auto frames =
                AssetPaths::collectFramesByPrefix(// Colecciona los frames por prefijo
                    "enemies/skeleton",// Carpeta de los skeletons
                    prefix
                );

            Animation clip =
                AssetPaths::buildAnimation(// Construye la animacion
                    frames,
                    state == "attack" ? 0.08f :
                    state == "death" ? 0.14f :
                    0.12f,

                    state != "death"
                );

            anim.registerAnimation(
                state + "_" + dir,
                clip
            );
        }
    }
}
// Elige el clip de la animacion
std::string DirectionalEnemyAnimator::pickClip(
    const Enemy& enemy
) const {
// Convierte la direccion a un string
    std::string dir =
        directionToString(
            enemy.getFacingDirection()
        );

    if (enemy.isDeathAnimPending()) {
        return "death_" + dir;
    }

    if (enemy.getAIState() == EnemyState::Attack) {
        return "attack_" + dir;
    }

    sf::Vector2f vel =
        enemy.getVelocity();

    float speed =
        std::sqrt(
            vel.x * vel.x +// Velocidad x al cuadrado
            vel.y * vel.y// Velocidad y al cuadrado
        );

    // NO diagonal real
    // prioriza eje dominante

    if (speed > 5.f) {
        return "walk_" + dir;// Walk
    }

    return "idle_" + dir;// Idle
}

void DirectionalEnemyAnimator::update(
    Enemy& enemy,
    float dt
) {

    std::string clip =
        pickClip(enemy);

    if (clip != lastClip) {
// Reproduce la animacion
        anim.play(
            clip,
            enemy.getAIState() == EnemyState::Attack
        );
// Establece el ultimo clip
        lastClip = clip;
    }

    anim.update(dt);// Actualiza la animacion
}

void DirectionalEnemyAnimator::applyToEntity(
    Entity& entity,
    float scale
) {

    anim.applyToSprite(
        entity.getSprite(),
        scale// Escala del sprite
    );

    entity.getSprite().setPosition(
        entity.getPosition()
    );
}
