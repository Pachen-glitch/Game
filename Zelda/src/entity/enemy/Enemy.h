#pragma once

#include "../base/Entity.h"
#include "EnemyState.h"

#include "../../utils/Timer.h"

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>

class Map;
class Player;

// Base enemy — Zelda-style wander/chase state machine with spawn memory.
class Enemy : public Entity {
public:
    Enemy(sf::Vector2f pos, EnemyKind kind, int hp, const std::string& sprite);

    void update(float dt) override;
    void onInteract(Player& player) override;

    virtual void think(Player& player, float dt, const Map& map);

    virtual void takeHit(int damage, sf::Vector2f knockback);

    bool isDead() const;
    int getHealth() const;

    EnemyState getAIState() const;
    EnemyKind getKind() const;

    Direction getFacingDirection() const { return facingDirection; }

    sf::FloatRect getAttackBounds() const;
    virtual sf::FloatRect getContactBounds() const;

    float getContactDamage() const { return contactDamage; }
    float getContactKnockback() const { return contactKnockback; }

    bool canDealContactDamage() const {
        return contactCooldown.finished();
    }

    void resetContactCooldown(float duration = 0.9f);

    void damage(int amount);

    sf::Vector2f getVelocity() const { return velocity; }

    bool isHurtAnimating() const {
        return hurtAnimTimer.isActive();
    }

    bool isDeathAnimPending() const {
        return deathAnimPending;
    }

protected:
    void setAIState(EnemyState s);

    bool seesPlayer(const Player& player) const;

    void applyMovement(float dt, const Map& map);

    virtual void initKindStats();

    virtual void modifyVelocity(float dt);

    void updateIdle(float dt, const Map& map);

    void updateWander(float dt, const Map& map);

    void updateChase(const Player& player, float dt);

    void updateReturnHome(float dt);

    void updateAttack(const Player& player, float dt);

    void pickWanderDirection(const Map& map);

    float randomRange(float lo, float hi) const;

    int health = 2;
    int maxHealth = 2;

    EnemyState aiState = EnemyState::Idle;
    EnemyKind kind = EnemyKind::Slime;

    Direction facingDirection = Direction::DOWN;

    sf::Vector2f velocity;
    sf::Vector2f spawnPosition;
    sf::Vector2f wanderDirection;

    float moveSpeed = 90.f;
    float chaseSpeed = 0.f;

    Timer attackCooldown;
    Timer aggroMemory;
    Timer stateTimer;
    Timer hurtAnimTimer;
    Timer contactCooldown;

    float aggroRadius = 160.f;
    float deaggroRadius = 240.f;
    float aggroMemoryDuration = 3.f;
    float maxChaseFromSpawn = 220.f;

    float contactDamage = 1.f;
    float contactKnockback = 120.f;

    bool deathAnimPending = false;
};