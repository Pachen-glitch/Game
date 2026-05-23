#include "Enemy.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>

#include "../../core/CombatFeel.h"
#include "../../core/Constants.h"
#include "../../interaction/EventBus.h"
#include "../../map/Map.h"
#include "../../movement/CollisionSystem.h"
#include "../../movement/EnemyMovement.h"
#include "../../utils/MathUtils.h"
#include "../player/Player.h"

namespace {
float rand01() {
    return static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
}
}

Enemy::Enemy(sf::Vector2f pos, EnemyKind k, int hp, const std::string& sprite)
    : Entity(pos, {48.f, 48.f}, sprite, EntityType::Enemy)
    , health(hp)
    , maxHealth(hp)
    , kind(k)
{
    spawnPosition = pos;
    initKindStats();
    stateTimer.start(randomRange(0.8f, 2.2f));
    setAIState(EnemyState::Idle);
}

void Enemy::initKindStats() {
    aggroRadius = Constants::AGGRO_RADIUS;
    deaggroRadius = Constants::DEAGGRO_RADIUS;
    aggroMemoryDuration = 3.f;
    maxChaseFromSpawn = 220.f;
    moveSpeed = Constants::ENEMY_SPEED;
    chaseSpeed = 0.f;
}

void Enemy::update(float dt) {
    if (deathAnimPending) {
        velocity = {0.f, 0.f};
    }

    attackCooldown.tick(dt);
    aggroMemory.tick(dt);
    stateTimer.tick(dt);
    contactCooldown.tick(dt);
    hurtAnimTimer.tick(dt);

    if (hurtAnimTimer.isActive()) {
        velocity *= 0.82f;
    } else if (!deathAnimPending) {
        velocity *= 0.88f;
    }
}

void Enemy::onInteract(Player& player) {
    if (!isDead()) player.damage(1);
}

void Enemy::think(Player& player, float dt, const Map& map) {
    if (deathAnimPending || isDead()) return;

    if (hurtAnimTimer.isActive()) {
        setAIState(EnemyState::Hurt);
        applyMovement(dt, map);
        return;
    }

    float distPlayer = MathUtils::distance(getPosition(), player.getPosition());
    float distSpawn = MathUtils::distance(getPosition(), spawnPosition);

    if (seesPlayer(player)) {
        aggroMemory.start(aggroMemoryDuration);
    }

    bool hasAggro = !aggroMemory.finished();
    bool inChaseRange = distPlayer <= deaggroRadius;
    bool tooFarFromHome = distSpawn > maxChaseFromSpawn;

    if (hasAggro && inChaseRange && !tooFarFromHome) {
        if (distPlayer <= Constants::ENEMY_ATTACK_RANGE) {
            setAIState(EnemyState::Attack);
        } else {
            setAIState(EnemyState::Chase);
        }
    } else if (tooFarFromHome && (hasAggro || aiState == EnemyState::Chase ||
                                  aiState == EnemyState::Attack)) {
        setAIState(EnemyState::ReturnToSpawn);
    } else if (!hasAggro || !inChaseRange) {
        if (distSpawn > 20.f &&
            (aiState == EnemyState::Chase || aiState == EnemyState::Attack)) {
            setAIState(EnemyState::ReturnToSpawn);
        } else if (aiState == EnemyState::Chase ||
                   aiState == EnemyState::Attack ||
                   aiState == EnemyState::ReturnToSpawn) {
            if (distSpawn <= 16.f) {
                setAIState(EnemyState::Idle);
                stateTimer.start(randomRange(0.5f, 1.5f));
            }
        }
    }

    switch (aiState) {
        case EnemyState::Idle:
            updateIdle(dt, map);
            break;
        case EnemyState::Wander:
            updateWander(dt, map);
            break;
        case EnemyState::Chase:
            updateChase(player, dt);
            break;
        case EnemyState::ReturnToSpawn:
            updateReturnHome(dt);
            break;
        case EnemyState::Attack:
            updateAttack(player, dt);
            break;
        default:
            velocity = {0.f, 0.f};
            break;
    }

    modifyVelocity(dt);
    applyMovement(dt, map);
}

void Enemy::takeHit(int damage, sf::Vector2f knockback) {
    if (isDead() || deathAnimPending) return;

    health -= damage;
    velocity = knockback * 1.35f;
    CombatFeel::instance().triggerHitPause(0.05f);
    CombatFeel::instance().setEnemyFlashTimer(0.14f);
    EventBus::instance().emit("enemy_hit");

    if (health <= 0) {
        setAIState(EnemyState::Dead);
        deathAnimPending = true;
        velocity = {0.f, 0.f};
        EventBus::instance().emit("enemy_died");
    } else {
        setAIState(EnemyState::Hurt);
        hurtAnimTimer.start(0.36f);
    }
}

bool Enemy::isDead() const {
    return !isActive() || health <= 0;
}

int Enemy::getHealth() const { return health; }
EnemyState Enemy::getAIState() const { return aiState; }
EnemyKind Enemy::getKind() const { return kind; }

sf::FloatRect Enemy::getAttackBounds() const {
    return getContactBounds();
}

sf::FloatRect Enemy::getContactBounds() const {
    return getBounds();
}

void Enemy::resetContactCooldown(float duration) {
    contactCooldown.start(duration);
}

void Enemy::damage(int amount) {
    takeHit(amount, {0.f, 0.f});
}

void Enemy::setAIState(EnemyState s) {
    aiState = s;
}

bool Enemy::seesPlayer(const Player& player) const {
    return MathUtils::distance(getPosition(), player.getPosition()) <= aggroRadius;
}

void Enemy::applyMovement(float dt, const Map& map) {
    sf::Vector2f next = EnemyMovement::moveWithCollision(
        map, getPosition(), velocity, dt, getSize()
    );
    setPosition(next);
}

void Enemy::updateIdle(float dt, const Map& map) {
    (void)dt;
    velocity = {0.f, 0.f};

    if (stateTimer.finished()) {
        pickWanderDirection(map);
        setAIState(EnemyState::Wander);
        stateTimer.start(randomRange(0.28f, 0.65f));
    }
}

void Enemy::updateWander(float dt, const Map& map) {
    (void)map;
    velocity = wanderDirection * (moveSpeed * 0.55f);

    if (stateTimer.finished()) {
        setAIState(EnemyState::Idle);
        stateTimer.start(randomRange(0.7f, 2.0f));
        velocity = {0.f, 0.f};
    }
}

void Enemy::updateChase(const Player& player, float dt) {
    (void)dt;
    float speed = chaseSpeed > 0.f ? chaseSpeed : moveSpeed;
    sf::Vector2f dir = MathUtils::directionTo(
        getPosition(), player.getPosition()
    );
    velocity = dir * speed;
}

void Enemy::updateReturnHome(float dt) {
    (void)dt;
    float dist = MathUtils::distance(getPosition(), spawnPosition);
    if (dist < 14.f) {
        setAIState(EnemyState::Idle);
        stateTimer.start(randomRange(0.6f, 1.4f));
        velocity = {0.f, 0.f};
        return;
    }
    velocity = MathUtils::directionTo(getPosition(), spawnPosition) * (moveSpeed * 0.7f);
}

void Enemy::updateAttack(const Player& player, float dt) {
    (void)player;
    (void)dt;
    velocity = {0.f, 0.f};
    if (attackCooldown.finished()) {
        attackCooldown.start(1.1f);
    }
}

void Enemy::pickWanderDirection(const Map& map) {
    static const sf::Vector2f dirs[] = {
        {0.f, -1.f}, {0.f, 1.f}, {-1.f, 0.f}, {1.f, 0.f}
    };

    for (int attempt = 0; attempt < 6; ++attempt) {
        int idx = static_cast<int>(rand01() * 4.f) % 4;
        sf::Vector2f dir = dirs[idx];
        sf::Vector2f probe = getPosition() + dir * static_cast<float>(Constants::TILE_SIZE);
        if (!CollisionSystem::isWall(map, probe)) {
            wanderDirection = dir;
            return;
        }
    }
    wanderDirection = dirs[static_cast<int>(rand01() * 4.f) % 4];
}

void Enemy::modifyVelocity(float dt) {
    (void)dt;
}

float Enemy::randomRange(float lo, float hi) const {
    return lo + (hi - lo) * rand01();
}
