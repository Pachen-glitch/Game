#pragma once

#include "../base/Entity.h"
#include "Direction.h"
#include "PlayerState.h"
#include "PlayerStats.h"

#include "../../utils/Timer.h"

// Player entity — stats, combat states, animation-facing flags.
class Player : public Entity {
public:
    Player(sf::Vector2f startPos);

    void update(float deltaTime) override;
    void onInteract(Player& player) override;

    // Movement
    void setVelocity(sf::Vector2f vel);
    sf::Vector2f getVelocity() const;
    void setDirection(Direction dir);
    Direction getDirection() const;
    bool isMoving() const;

    // Combat actions
    bool trySwordAttack();
    bool trySpinAttack();
    void setShieldHeld(bool held);
    bool isShieldActive() const;
    void applyKnockback(sf::Vector2f force);

    void damage(int amount);
    bool canTakeDamage() const;

    // Pickups / economy
    void addRupees(int amount);
    void addKey();
    bool useKey();
    void heal(int amount);

    // Getters
    PlayerStats& getStats();
    const PlayerStats& getStats() const;
    PlayerState getState() const;
    float getSwordDamage() const;

    // Legacy accessors used by items
    int getLives() const;
    int getCoins() const;
    int getKeys() const;

    void swordAttack();
    void spinAttack();
    void activateShield();
    void deactivateShield();

    bool moving = false;

private:
    void updateStateTimers(float dt);
    void setState(PlayerState state);

    PlayerStats stats;
    PlayerState state = PlayerState::Idle;
    Direction direction = Direction::DOWN;
    sf::Vector2f velocity;

    Timer invulnTimer;
    Timer attackTimer;
    Timer spinTimer;
    Timer hurtTimer;

    bool shieldHeld = false;
    bool attacking = false;
    bool spinning = false;
};
