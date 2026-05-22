#pragma once

#include "../base/Entity.h"
#include "EnemyState.h"

#include "../../utils/Timer.h"

#include <SFML/Graphics/Rect.hpp>

class Player;

// Base enemy — state machine, health, aggro memory.
class Enemy : public Entity {
public:
    Enemy(sf::Vector2f pos, EnemyKind kind, int hp, const std::string& sprite);

    void update(float dt) override;
    void onInteract(Player& player) override;

    virtual void think(Player& player, float dt);

    virtual void takeHit(int damage, sf::Vector2f knockback);
    bool isDead() const;
    int getHealth() const;

    EnemyState getAIState() const;
    EnemyKind getKind() const;

    sf::FloatRect getAttackBounds() const;

    void damage(int amount); // legacy

protected:
    void setAIState(EnemyState s);
    void patrol(float dt);
    void chase(Player& player, float dt, float speed);
    bool seesPlayer(Player& player) const;
    void loseAggro(float dt);

    int health = 2;
    int maxHealth = 2;
    EnemyState aiState = EnemyState::Patrol;
    EnemyKind kind = EnemyKind::Slime;

    sf::Vector2f velocity;
    sf::Vector2f patrolOrigin;
    sf::Vector2f patrolTarget;
    float moveSpeed = 90.f;

    Timer attackCooldown;
    Timer aggroMemory;
    Timer stateTimer;

    float aggroRadius = 160.f;
    float deaggroRadius = 240.f;
};
