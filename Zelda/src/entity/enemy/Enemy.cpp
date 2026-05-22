#include "Enemy.h"

#include <algorithm>
#include "../../core/Constants.h"
#include "../../interaction/EventBus.h"
#include "../../utils/MathUtils.h"
#include "../player/Player.h"

Enemy::Enemy(sf::Vector2f pos, EnemyKind k, int hp, const std::string& sprite)
    : Entity(pos, {48.f, 48.f}, sprite, EntityType::Enemy)
    , health(hp)
    , maxHealth(hp)
    , kind(k)
{
    patrolOrigin = pos;
    patrolTarget = pos + sf::Vector2f(64.f, 0.f);
    aggroRadius = Constants::AGGRO_RADIUS;
    deaggroRadius = Constants::DEAGGRO_RADIUS;
}

void Enemy::update(float dt) {
    if (isDead()) return;
    setPosition(position + velocity * dt);
    velocity *= 0.9f;
    attackCooldown.tick(dt);
    aggroMemory.tick(dt);
    stateTimer.tick(dt);
}

void Enemy::onInteract(Player& player) {
    if (!isDead()) player.damage(1);
}

void Enemy::think(Player& player, float dt) {
    if (isDead()) return;

    if (seesPlayer(player)) {
        aggroMemory.start(3.f);
        setAIState(EnemyState::Chase);
    } else if (aggroMemory.finished()) {
        loseAggro(dt);
    }

    switch (aiState) {
        case EnemyState::Patrol:
            patrol(dt);
            break;
        case EnemyState::Chase:
            chase(player, dt, moveSpeed);
            if (MathUtils::distance(getPosition(), player.getPosition()) < Constants::ENEMY_ATTACK_RANGE) {
                setAIState(EnemyState::Attack);
            }
            break;
        case EnemyState::Attack:
            velocity = {0.f, 0.f};
            if (attackCooldown.finished()) {
                attackCooldown.start(1.2f);
            }
            if (!seesPlayer(player)) setAIState(EnemyState::Patrol);
            break;
        default:
            break;
    }
}

void Enemy::takeHit(int damage, sf::Vector2f knockback) {
    if (isDead()) return;
    health -= damage;
    velocity = knockback;
    EventBus::instance().emit("enemy_hit");

    if (health <= 0) {
        setAIState(EnemyState::Dead);
        deactivate();
        EventBus::instance().emit("enemy_died");
    }
}

bool Enemy::isDead() const {
    return !isActive() || health <= 0;
}

int Enemy::getHealth() const { return health; }
EnemyState Enemy::getAIState() const { return aiState; }
EnemyKind Enemy::getKind() const { return kind; }

sf::FloatRect Enemy::getAttackBounds() const {
    sf::Vector2f c = getPosition() + getSize() * 0.5f;
    float r = 20.f;
    return sf::FloatRect(c.x - r, c.y - r, r * 2.f, r * 2.f);
}

void Enemy::damage(int amount) {
    takeHit(amount, {0.f, 0.f});
}

void Enemy::setAIState(EnemyState s) { aiState = s; }

void Enemy::patrol(float dt) {
    sf::Vector2f dir = MathUtils::directionTo(getPosition(), patrolTarget);
    velocity = dir * moveSpeed;

    if (MathUtils::distance(getPosition(), patrolTarget) < 8.f) {
        std::swap(patrolOrigin, patrolTarget);
        patrolTarget = patrolOrigin + sf::Vector2f(
            (patrolTarget.x > patrolOrigin.x) ? -80.f : 80.f, 0.f
        );
    }
}

void Enemy::chase(Player& player, float dt, float speed) {
    sf::Vector2f dir = MathUtils::directionTo(
        getPosition(), player.getPosition()
    );
    velocity = dir * speed;
}

bool Enemy::seesPlayer(Player& player) const {
    return MathUtils::distance(getPosition(), player.getPosition()) <= aggroRadius;
}

void Enemy::loseAggro(float dt) {
    if (MathUtils::distance(getPosition(), patrolOrigin) > deaggroRadius) {
        setAIState(EnemyState::Patrol);
    } else if (aiState == EnemyState::Chase) {
        setAIState(EnemyState::Patrol);
    }
}
