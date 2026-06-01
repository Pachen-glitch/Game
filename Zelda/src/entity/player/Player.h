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

    void setVelocity(sf::Vector2f vel);
    sf::Vector2f getVelocity() const;
    sf::FloatRect getBounds() const override;

    void setDirection(Direction dir);
    Direction getDirection() const;
    Direction getFacingDirection() const;
    bool isMoving() const;
    void updateLocomotionState(bool isMovingInput);

    bool trySwordAttack();
    bool trySpinAttack();
    bool tryActivateBerserk();
    void setShieldHeld(bool held);
    bool isShieldActive() const;
    bool isBerserkActive() const;
    void breakShieldForAttack();
    void increaseMaxHearts(float amount = 1.f);
    float getPersistedMaxHearts() const;
    void applyKnockback(sf::Vector2f force);

    void damage(float amount);
    void damage(int amount);
    bool canTakeDamage() const;

    static bool isDebugGodMode();
    static void setDebugGodMode(bool enabled);

    void addRupees(int amount);
    void addKey();
    bool useKey();
    void heal(int amount);

    PlayerStats& getStats();
    const PlayerStats& getStats() const;
    PlayerState getState() const;
    float getSwordDamage() const;

    int getLives() const;
    int getCoins() const;
    int getKeys() const;

    void swordAttack();
    void spinAttack();
    void activateShield();
    void deactivateShield();

    bool isAttacking() const;
    bool canMove() const;
    bool shouldSpawnSwordHit() const;
    void markSwordHitSpawned();

    bool moving = false;

    float getMoveSpeed() const;

private:
    void updateStateTimers(float dt);
    void setState(PlayerState state);
    void updateBerserk(float dt);
    void endBerserk();

    PlayerStats stats;
    PlayerState state = PlayerState::Idle;
    Direction direction = Direction::DOWN;
    Direction lockedAttackDirection = Direction::DOWN;
    sf::Vector2f velocity;

    Timer invulnTimer;
    Timer attackTimer;
    Timer spinTimer;
    Timer hurtTimer;
    Timer swordCooldown;

    bool shieldHeld = false;
    bool attacking = false;
    bool spinning = false;
    bool swordHitboxSpawned = false;
    bool berserkActive = false;

    float berserkStoredMaxHearts = 0.f;
    Timer berserkTimer;
    Timer berserkCooldownTimer;
};
