#include "Player.h"
#include "../../core/Constants.h"
#include "../../interaction/EventBus.h"
#include "../../save/RunScoreTracker.h"
#include "../../utils/AssetPaths.h"

#include <algorithm>

namespace {

float healthRatio(float hearts, float maxHearts) {
    if (maxHearts <= 0.f) return 0.f;
    return hearts / maxHearts;
}

bool gDebugGodMode = false;

} // namespace

Player::Player(sf::Vector2f startPos)
    : Entity(
        startPos,
        {static_cast<float>(Constants::TILE_SIZE), static_cast<float>(Constants::TILE_SIZE)},
        AssetPaths::getPlayerIdleSprite(Direction::DOWN),
        EntityType::Player
    )
{
    stats.maxHearts = static_cast<float>(Constants::STARTING_HEARTS);
    stats.hearts = stats.maxHearts;
    stats.rupees = Constants::STARTING_RUPEES;
}

void Player::update(float deltaTime) {
    updateStateTimers(deltaTime);
    updateBerserk(deltaTime);

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
    // Movimiento manejado por MovementSystem
    }
}

void Player::onInteract(Player&) {}

void Player::setVelocity(sf::Vector2f vel) { velocity = vel; }
sf::Vector2f Player::getVelocity() const { return velocity; }
sf::FloatRect Player::getBounds() const {

    float width = size.x * 0.55f;
    float height = size.y * 0.45f;

    float offsetX = (size.x - width) * 0.5f;
    float offsetY = size.y - height;

    return {
        position.x + offsetX,
        position.y + offsetY,
        width,
        height
    };
}


void Player::setDirection(Direction dir) { direction = dir; }
Direction Player::getDirection() const { return direction; }

Direction Player::getFacingDirection() const {
    if (state == PlayerState::Attack) return lockedAttackDirection;
    return direction;
}

bool Player::isMoving() const {
    return moving && (state == PlayerState::Walk || state == PlayerState::Idle);
}

void Player::updateLocomotionState(bool isMovingInput) {
    moving = isMovingInput;
    if (!canMove()) return;
    if (moving) {
        setState(PlayerState::Walk);
    } else if (state == PlayerState::Walk) {
        setState(PlayerState::Idle);
    }
}

bool Player::trySwordAttack() {
    if (!swordCooldown.finished()) return false;
    if (state == PlayerState::Attack || state == PlayerState::Spin ||
        state == PlayerState::Hurt || state == PlayerState::Dead ||
        state == PlayerState::Shield) {
        return false;
    }
    swordAttack();
    return true;
}

bool Player::trySpinAttack() {
    if (!stats.hasSpinAttack) return false;
    if (state == PlayerState::Attack || state == PlayerState::Spin ||
        state == PlayerState::Hurt || state == PlayerState::Dead ||
        state == PlayerState::Shield) {
        return false;
    }
    spinAttack();
    return true;
}

bool Player::tryActivateBerserk() {
    if (berserkActive || !berserkCooldownTimer.finished()) return false;
    if (state == PlayerState::Attack || state == PlayerState::Spin ||
        state == PlayerState::Hurt || state == PlayerState::Dead) {
        return false;
    }

    float ratio = healthRatio(stats.hearts, stats.maxHearts);
    berserkStoredMaxHearts = stats.maxHearts;
    stats.maxHearts *= 0.5f;
    stats.hearts = ratio * stats.maxHearts;

    stats.swordDamage *= Constants::BERSERK_DAMAGE_MULT;
    stats.moveSpeedMult *= Constants::BERSERK_MOVE_MULT;
    stats.attackSpeedMult *= Constants::BERSERK_ATTACK_SPEED_MULT;

    berserkActive = true;
    berserkTimer.start(Constants::BERSERK_DURATION);
    deactivateShield();
    shieldHeld = false;
    return true;
}

void Player::updateBerserk(float dt) {
    berserkCooldownTimer.tick(dt);
    if (!berserkActive) return;

    berserkTimer.tick(dt);
    if (berserkTimer.finished()) {
        endBerserk();
    }
}

void Player::endBerserk() {
    if (!berserkActive) return;

    float ratio = healthRatio(stats.hearts, stats.maxHearts);
    stats.maxHearts = berserkStoredMaxHearts;
    stats.hearts = ratio * stats.maxHearts;

    stats.swordDamage /= Constants::BERSERK_DAMAGE_MULT;
    stats.moveSpeedMult /= Constants::BERSERK_MOVE_MULT;
    stats.attackSpeedMult /= Constants::BERSERK_ATTACK_SPEED_MULT;

    berserkActive = false;
    berserkCooldownTimer.start(Constants::BERSERK_COOLDOWN);
}

void Player::setShieldHeld(bool held) {
    shieldHeld = held;
    if (held && stats.hasShield && state != PlayerState::Attack &&
        state != PlayerState::Spin && state != PlayerState::Hurt &&
        state != PlayerState::Dead) {
        activateShield();
    } else if (!held) {
        deactivateShield();
    }
}

void Player::breakShieldForAttack() {
    shieldHeld = false;
    deactivateShield();
}

bool Player::isShieldActive() const {
    return shieldHeld && stats.hasShield && state == PlayerState::Shield;
}

bool Player::isBerserkActive() const {
    return berserkActive;
}

void Player::increaseMaxHearts(float amount) {
    if (berserkActive) {
        berserkStoredMaxHearts += amount;
        stats.maxHearts += amount * 0.5f;
    } else {
        stats.maxHearts += amount;
    }
    stats.heal(amount);
}

float Player::getPersistedMaxHearts() const {
    if (berserkActive) {
        return berserkStoredMaxHearts;
    }
    return stats.maxHearts;
}

void Player::applyKnockback(sf::Vector2f force) {
    velocity = force;
    setState(PlayerState::Hurt);
    hurtTimer.start(0.25f);
}

void Player::damage(float amount) {
    if (gDebugGodMode || !canTakeDamage()) return;

    if (isShieldActive()) {
        amount *= Constants::SHIELD_DAMAGE_MULT;
    }

    stats.takeDamage(amount);
    invulnTimer.start(Constants::INVULN_DURATION);
    EventBus::instance().emit("player_damaged");

    if (stats.isDead()) {
        if (berserkActive) {
            endBerserk();
        }
        setState(PlayerState::Dead);
        EventBus::instance().emit("player_died");
    } else {
        setState(PlayerState::Hurt);
        hurtTimer.start(0.3f);
    }
}

bool Player::canTakeDamage() const {
    if (gDebugGodMode) return false;
    return invulnTimer.finished() && state != PlayerState::Dead;
}

bool Player::isDebugGodMode() {
    return gDebugGodMode;
}

void Player::setDebugGodMode(bool enabled) {
    gDebugGodMode = enabled;
}

void Player::addRupees(int amount) {
    stats.rupees += amount;
    if (RunScoreTracker* tracker = RunScoreTracker::active()) {
        tracker->onRupeeCollected(amount);
    }
    EventBus::instance().emit("rupee_pickup");
}

void Player::addKey() {
    stats.keys++;
    if (RunScoreTracker* tracker = RunScoreTracker::active()) {
        tracker->onKeyCollected();
    }
}

bool Player::useKey() {
    if (stats.keys <= 0) return false;
    stats.keys--;
    return true;
}

void Player::damage(int amount) {
    damage(static_cast<float>(amount));
}

void Player::heal(int amount) {
    stats.heal(static_cast<float>(amount));
    EventBus::instance().emit("heart_pickup");
}

PlayerStats& Player::getStats() { return stats; }
const PlayerStats& Player::getStats() const { return stats; }
PlayerState Player::getState() const { return state; }
float Player::getSwordDamage() const { return stats.swordDamage; }

int Player::getLives() const {
    return static_cast<int>(stats.hearts + 0.999f);
}
int Player::getCoins() const { return stats.rupees; }
int Player::getKeys() const { return stats.keys; }

void Player::swordAttack() {
    deactivateShield();
    attacking = true;
    swordHitboxSpawned = false;
    lockedAttackDirection = direction;
    setState(PlayerState::Attack);
    float duration = Constants::ATTACK_DURATION / stats.attackSpeedMult;
    attackTimer.start(duration);
    EventBus::instance().emit("player_attack");
}

bool Player::isAttacking() const {
    return state == PlayerState::Attack;
}

bool Player::canMove() const {
    return state != PlayerState::Attack && state != PlayerState::Spin &&
           state != PlayerState::Hurt && state != PlayerState::Dead;
}

bool Player::shouldSpawnSwordHit() const {
    if (state != PlayerState::Attack || swordHitboxSpawned) return false;
    float elapsed = attackTimer.getDuration() - attackTimer.getRemaining();
    return elapsed >= Constants::ATTACK_HIT_FRAME_TIME;
}

void Player::markSwordHitSpawned() {
    swordHitboxSpawned = true;
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
    swordCooldown.tick(dt);

    if (attackTimer.finished() && state == PlayerState::Attack) {
        attacking = false;
        setState(PlayerState::Idle);
        swordCooldown.start(Constants::SWORD_COOLDOWN / stats.attackSpeedMult);
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

float Player::getMoveSpeed() const {
    return Constants::PLAYER_SPEED *
           stats.moveSpeedMult;
}