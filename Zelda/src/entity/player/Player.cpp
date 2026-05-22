#include "Player.h"
#include "../../core/Constants.h"
#include "../../interaction/EventBus.h"

Player::Player(sf::Vector2f startPos)
    : Entity(
        startPos,
        {static_cast<float>(Constants::TILE_SIZE), static_cast<float>(Constants::TILE_SIZE)},
        "assets/sprites/player/move/walk_d1.png",
        EntityType::Player
    )
{
    stats.maxHearts = Constants::STARTING_HEARTS;
    stats.hearts = Constants::STARTING_HEARTS;
    stats.rupees = Constants::STARTING_RUPEES;
}

void Player::update(float deltaTime) {
    updateStateTimers(deltaTime);

    if (stats.isDead()) {
        setState(PlayerState::Dead);
        return;
    }

    if (state == PlayerState::Hurt || state == PlayerState::Attack ||
        state == PlayerState::Spin) {
        setPosition(position + velocity * deltaTime);
        velocity *= 0.85f;
        return;
    }

    if (state != PlayerState::Shield) {
        setPosition(position + velocity * deltaTime);
    }
}

void Player::onInteract(Player&) {}

void Player::setVelocity(sf::Vector2f vel) { velocity = vel; }
sf::Vector2f Player::getVelocity() const { return velocity; }

void Player::setDirection(Direction dir) { direction = dir; }
Direction Player::getDirection() const { return direction; }

bool Player::isMoving() const {
    return moving && (state == PlayerState::Walk || state == PlayerState::Idle);
}

bool Player::trySwordAttack() {
    if (state == PlayerState::Attack || state == PlayerState::Spin ||
        state == PlayerState::Hurt || state == PlayerState::Dead) {
        return false;
    }
    swordAttack();
    return true;
}

bool Player::trySpinAttack() {
    if (!stats.hasSpinAttack) return false;
    if (state == PlayerState::Attack || state == PlayerState::Spin ||
        state == PlayerState::Hurt || state == PlayerState::Dead) {
        return false;
    }
    spinAttack();
    return true;
}

void Player::setShieldHeld(bool held) {
    shieldHeld = held;
    if (held && stats.hasShield) {
        activateShield();
    } else {
        deactivateShield();
    }
}

bool Player::isShieldActive() const {
    return shieldHeld && stats.hasShield && state == PlayerState::Shield;
}

void Player::applyKnockback(sf::Vector2f force) {
    velocity = force;
    setState(PlayerState::Hurt);
    hurtTimer.start(0.25f);
}

void Player::damage(int amount) {
    if (!canTakeDamage()) return;
    if (isShieldActive()) return;

    stats.takeDamage(amount);
    invulnTimer.start(Constants::INVULN_DURATION);
    EventBus::instance().emit("player_damaged");

    if (stats.isDead()) {
        setState(PlayerState::Dead);
        EventBus::instance().emit("player_died");
    } else {
        setState(PlayerState::Hurt);
        hurtTimer.start(0.3f);
    }
}

bool Player::canTakeDamage() const {
    return invulnTimer.finished() && state != PlayerState::Dead;
}

void Player::addRupees(int amount) {
    stats.rupees += amount;
    EventBus::instance().emit("rupee_pickup");
}

void Player::addKey() { stats.keys++; }

bool Player::useKey() {
    if (stats.keys <= 0) return false;
    stats.keys--;
    return true;
}

void Player::heal(int amount) {
    stats.heal(amount);
    EventBus::instance().emit("heart_pickup");
}

PlayerStats& Player::getStats() { return stats; }
const PlayerStats& Player::getStats() const { return stats; }
PlayerState Player::getState() const { return state; }
float Player::getSwordDamage() const { return stats.swordDamage; }

int Player::getLives() const { return stats.hearts; }
int Player::getCoins() const { return stats.rupees; }
int Player::getKeys() const { return stats.keys; }

void Player::swordAttack() {
    attacking = true;
    setState(PlayerState::Attack);
    attackTimer.start(Constants::ATTACK_DURATION / stats.attackSpeedMult);
    EventBus::instance().emit("player_attack");
}

void Player::spinAttack() {
    spinning = true;
    setState(PlayerState::Spin);
    spinTimer.start(Constants::SPIN_DURATION);
    EventBus::instance().emit("player_spin");
}

void Player::activateShield() { setState(PlayerState::Shield); }
void Player::deactivateShield() {
    if (state == PlayerState::Shield) setState(PlayerState::Idle);
}

void Player::updateStateTimers(float dt) {
    invulnTimer.tick(dt);
    attackTimer.tick(dt);
    spinTimer.tick(dt);
    hurtTimer.tick(dt);

    if (attackTimer.finished() && state == PlayerState::Attack) {
        attacking = false;
        setState(PlayerState::Idle);
    }
    if (spinTimer.finished() && state == PlayerState::Spin) {
        spinning = false;
        setState(PlayerState::Idle);
    }
    if (hurtTimer.finished() && state == PlayerState::Hurt) {
        setState(PlayerState::Idle);
    }
}

void Player::setState(PlayerState newState) {
    state = newState;
}
