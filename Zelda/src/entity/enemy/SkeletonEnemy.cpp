#include "SkeletonEnemy.h"

#include "../player/Player.h"

#include "../../utils/AssetPaths.h"
#include "../../utils/MathUtils.h"

SkeletonEnemy::SkeletonEnemy(sf::Vector2f pos)
    : Enemy(
        pos,
        EnemyKind::Skeleton,
        5,
        AssetPaths::resolveFirst({
            "sprites/enemies/skeleton/idle/idle_d1.png"
        })
    )
{
    // más lento que player
    moveSpeed = 55.f;

    // persecución más lenta
    chaseSpeed = 70.f;

    // rango de detección
    aggroRadius = 240.f;
    deaggroRadius = 320.f;

    // distancia máxima
    maxChaseFromSpawn = 420.f;

    // daño
    contactDamage = 2.f;
    contactKnockback = 170.f;

    // ataque cada 1.5 segundos
    attackCooldown.start(1.5f);
}

void SkeletonEnemy::think(
    Player& player,
    float dt,
    const Map& map
) {

    Enemy::think(
        player,
        dt,
        map
    );

    // SOLO movimiento tipo WASD
    // elimina diagonales

    if (std::abs(velocity.x) >
        std::abs(velocity.y)) {

        velocity.y = 0.f;
    }
    else {

        velocity.x = 0.f;
    }

    // rage mode
    if (health <= 2 &&
        !isDead()) {

        chaseSpeed = 85.f;
    }
}

void SkeletonEnemy::takeHit(
    int damage,
    sf::Vector2f knockback
) {

    Enemy::takeHit(
        damage,
        knockback
    );

    if (!isDead()) {

        setAIState(
            EnemyState::Attack
        );

        attackCooldown.start(1.5f);
    }
}