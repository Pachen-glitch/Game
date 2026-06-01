#include "NarutoBoss.h"

#include "NarutoCloneEnemy.h"
#include "NarutoProjectile.h"
#include "../base/EntityManager.h"
#include "../player/Player.h"
#include "../../audio/AudioManager.h"
#include "../../interaction/EventBus.h"
#include "../../map/Map.h"
#include "../../utils/AssetPaths.h"
#include "../../utils/MathUtils.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <vector>

namespace {

float rand01() {
    return static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
}

struct WeightedAttack {
    NarutoAttack attack;
    float weight;
};

NarutoAttack pickWeighted(const std::vector<WeightedAttack>& pool) {
    float total = 0.f;
    for (const auto& entry : pool) {
        total += entry.weight;
    }
    if (total <= 0.f) return NarutoAttack::None;

    float roll = rand01() * total;
    for (const auto& entry : pool) {
        roll -= entry.weight;
        if (roll <= 0.f) return entry.attack;
    }
    return pool.back().attack;
}

} // namespace

NarutoBoss::NarutoBoss(sf::Vector2f pos)
    : Enemy(
        pos,
        EnemyKind::Naruto,
        48,
        AssetPaths::resolveFirst({
            "sprites/enemies/naruto/idle_l.png",
            "sprites/enemies/naruto/idle_r.png"
        })
    )
{
    moveSpeed = 95.f;
    chaseSpeed = 118.f;
    contactDamage = 1.5f;
    contactKnockback = 150.f;
    aggroRadius = 9999.f;
    deaggroRadius = 9999.f;
    maxChaseFromSpawn = 9999.f;
    mode = Mode::Intro;
    introTimer = 1.8f;
}

void NarutoBoss::updatePhaseFromHealth() {
    float ratio = static_cast<float>(health) / static_cast<float>(maxHealth);
    if (ratio > 0.7f) {
        phase = NarutoBossPhase::One;
    } else if (ratio > 0.3f) {
        phase = NarutoBossPhase::Two;
    } else {
        phase = NarutoBossPhase::Three;
    }
}

void NarutoBoss::tickCooldowns(float dt) {
    odamaCooldown = std::max(0.f, odamaCooldown - dt);
    rasenCooldown = std::max(0.f, rasenCooldown - dt);
    cloneCooldown = std::max(0.f, cloneCooldown - dt);
    substitutionCooldown = std::max(0.f, substitutionCooldown - dt);
    dashCooldown = std::max(0.f, dashCooldown - dt);
}

void NarutoBoss::updateFacing(const Player& player) {
    faceLeft = player.getPosition().x < getPosition().x;
    facingDirection = faceLeft ? Direction::LEFT : Direction::RIGHT;
}

void NarutoBoss::think(Player& player, float dt, const Map& map) {
    if (deathAnimPending || isDead()) return;

    tickCooldowns(dt);
    updatePhaseFromHealth();
    updateFacing(player);

    if (!musicStarted) {
        musicStarted = true;
        AudioManager::instance().playBossSpawnMusic();
    }

    if (hurtAnimTimer.isActive()) {
        currentAttack = NarutoAttack::None;
        kickActive = false;
        applyMovement(dt, map);
        return;
    }

    switch (mode) {
        case Mode::Intro:
            updateIntro(dt, map);
            return;
        case Mode::Recovery:
            updateRecovery(dt, map);
            return;
        case Mode::Chase:
            updateChaseMode(player, dt, map);
            return;
        case Mode::Executing:
            updateExecuting(player, dt, map);
            return;
    }
}

void NarutoBoss::updateIntro(float dt, const Map& map) {
    currentAttack = NarutoAttack::None;
    velocity = {0.f, 0.f};
    introTimer -= dt;

    if (!battleMusicStarted && introTimer < 0.6f) {
        battleMusicStarted = true;
        AudioManager::instance().playBossBattleMusic();
    }

    if (introTimer <= 0.f) {
        mode = Mode::Chase;
        startRecovery(0.6f);
    }

    applyMovement(dt, map);
}

void NarutoBoss::startRecovery(float duration) {
    mode = Mode::Recovery;
    recoveryTimer = duration;
    currentAttack = NarutoAttack::Retreat;
    velocity = {0.f, 0.f};
    kickActive = false;
}

void NarutoBoss::updateRecovery(float dt, const Map& map) {
    recoveryTimer -= dt;
    if (recoveryTimer <= 0.f) {
        mode = Mode::Chase;
        currentAttack = NarutoAttack::None;
        velocity = {0.f, 0.f};
    }
    applyMovement(dt, map);
}

void NarutoBoss::updateChaseMode(Player& player, float dt, const Map& map) {
    float dist = MathUtils::distance(getPosition(), player.getPosition());

    if (dist > 56.f) {
        sf::Vector2f dir = MathUtils::directionTo(getPosition(), player.getPosition());
        velocity = dir * chaseSpeed;
        currentAttack = NarutoAttack::None;
    } else {
        velocity = {0.f, 0.f};
        NarutoAttack next = pickNextAttack(player);
        if (next != NarutoAttack::None) {
            startAttack(next);
        } else {
            startRecovery(phase == NarutoBossPhase::Three ? 0.35f : 0.55f);
        }
    }

    applyMovement(dt, map);
}

bool NarutoBoss::isAttackReady(NarutoAttack attack) const {
    if (attack == lastAttack && sameAttackStreak >= 2) {
        return false;
    }

    switch (attack) {
        case NarutoAttack::Kick:
            return true;
        case NarutoAttack::Dash:
            return dashCooldown <= 0.f;
        case NarutoAttack::CloneSummon:
            return cloneCooldown <= 0.f && countActiveClones() < 3;
        case NarutoAttack::OdamaRasengan:
            return odamaCooldown <= 0.f;
        case NarutoAttack::RasenShuriken:
            return rasenCooldown <= 0.f;
        case NarutoAttack::Substitution:
            return phase == NarutoBossPhase::Three &&
                   substitutionCooldown <= 0.f;
        default:
            return false;
    }
}

void NarutoBoss::registerAttackUse(NarutoAttack attack) {
    if (attack == lastAttack) {
        ++sameAttackStreak;
    } else {
        lastAttack = attack;
        sameAttackStreak = 1;
    }
}

void NarutoBoss::stampAttackCooldown(NarutoAttack attack) {
    switch (attack) {
        case NarutoAttack::Dash:
            dashCooldown = phase == NarutoBossPhase::Three ? 2.5f : 3.5f;
            break;
        case NarutoAttack::CloneSummon:
            cloneCooldown = phase == NarutoBossPhase::Three ? 4.f : 6.f;
            break;
        case NarutoAttack::OdamaRasengan:
            odamaCooldown = 9.f;
            break;
        case NarutoAttack::RasenShuriken:
            rasenCooldown = 5.f;
            break;
        case NarutoAttack::Substitution:
            substitutionCooldown = 8.f;
            break;
        default:
            break;
    }
}

NarutoAttack NarutoBoss::pickNextAttack(const Player& player) {
    (void)player;

    std::vector<WeightedAttack> pool;

    auto tryAdd = [&](NarutoAttack attack, float weight) {
        if (isAttackReady(attack)) {
            pool.push_back({attack, weight});
        }
    };

    tryAdd(NarutoAttack::Kick, 3.f);

    if (phase == NarutoBossPhase::One) {
        tryAdd(NarutoAttack::Dash, 2.f);
    } else if (phase == NarutoBossPhase::Two) {
        tryAdd(NarutoAttack::Dash, 2.f);
        tryAdd(NarutoAttack::CloneSummon, 2.f);
        tryAdd(NarutoAttack::OdamaRasengan, 2.f);
        tryAdd(NarutoAttack::RasenShuriken, 2.f);
    } else {
        tryAdd(NarutoAttack::Dash, 2.5f);
        tryAdd(NarutoAttack::CloneSummon, 2.5f);
        tryAdd(NarutoAttack::OdamaRasengan, 2.f);
        tryAdd(NarutoAttack::RasenShuriken, 2.5f);
        tryAdd(NarutoAttack::Substitution, 2.f);
    }

    if (pool.empty()) {
        return NarutoAttack::None;
    }

    return pickWeighted(pool);
}

void NarutoBoss::startAttack(NarutoAttack attack) {
    registerAttackUse(attack);
    stampAttackCooldown(attack);

    mode = Mode::Executing;
    currentAttack = attack;
    attackTimer = 0.f;
    kickActive = false;
    velocity = {0.f, 0.f};

    switch (attack) {
        case NarutoAttack::Kick:
            attackDuration = 0.55f;
            break;
        case NarutoAttack::Dash:
            attackDuration = 0.35f;
            break;
        case NarutoAttack::CloneSummon:
            attackDuration = 0.9f;
            break;
        case NarutoAttack::OdamaRasengan:
            attackDuration = 1.1f;
            break;
        case NarutoAttack::RasenShuriken:
            attackDuration = 0.75f;
            break;
        case NarutoAttack::Substitution:
            attackDuration = 1.4f;
            substitutionDelay = 0.45f;
            hidden = false;
            break;
        default:
            attackDuration = 0.5f;
            break;
    }
}

void NarutoBoss::finishAttack() {
    kickActive = false;
    hidden = false;
    currentAttack = NarutoAttack::None;

    float recovery = 0.9f;
    if (phase == NarutoBossPhase::Two) recovery = 0.7f;
    if (phase == NarutoBossPhase::Three) recovery = 0.45f;

    if (lastAttack == NarutoAttack::Kick ||
        lastAttack == NarutoAttack::Dash) {
        recovery += 0.25f;
    }

    startRecovery(recovery);
}

void NarutoBoss::spawnClone(sf::Vector2f nearPos) {
    if (!entities || countActiveClones() >= 3) return;

    static const sf::Vector2f offsets[] = {
        {48.f, 0.f}, {-48.f, 0.f}, {0.f, 48.f}, {0.f, -48.f}
    };

    int idx = static_cast<int>(rand01() * 4.f) % 4;
    entities->spawn<NarutoCloneEnemy>(nearPos + offsets[idx]);
}

int NarutoBoss::countActiveClones() const {
    if (!entities) return 0;

    int count = 0;
    for (const auto& ent : entities->all()) {
        if (!ent || !ent->isActive()) continue;
        if (dynamic_cast<NarutoCloneEnemy*>(ent.get())) {
            ++count;
        }
    }
    return count;
}

void NarutoBoss::spawnOdama(const Player& player) {
    if (!entities) return;

    AudioManager::instance().playRasenganSound();

    sf::Vector2f dir = MathUtils::directionTo(getPosition(), player.getPosition());
    sf::Vector2f origin = getPosition() + dir * 40.f + sf::Vector2f(8.f, 8.f);

    entities->spawn<NarutoProjectile>(
        origin,
        dir * 95.f,
        NarutoProjectileType::Odama,
        3.f
    );
}

void NarutoBoss::spawnRasenShuriken(const Player& player, sf::Vector2f origin) {
    if (!entities) return;

    AudioManager::instance().playRasenShurikenSound();

    sf::Vector2f dir = MathUtils::directionTo(origin, player.getPosition());

    entities->spawn<NarutoProjectile>(
        origin,
        dir * 210.f,
        NarutoProjectileType::RasenShuriken,
        2.f
    );
}

void NarutoBoss::beginSubstitution(const Player& player) {
    sf::Vector2f toBoss = MathUtils::directionTo(
        player.getPosition(), getPosition()
    );
    setPosition(player.getPosition() + toBoss * 56.f);
    hidden = true;
    velocity = {0.f, 0.f};
    updateFacing(player);
}

void NarutoBoss::finishSubstitution(const Player& player) {
    hidden = false;
    spawnRasenShuriken(player, getPosition() + sf::Vector2f(8.f, 8.f));
}

void NarutoBoss::updateExecuting(Player& player, float dt, const Map& map) {
    attackTimer += dt;
    sf::Vector2f toPlayer = MathUtils::directionTo(getPosition(), player.getPosition());
    bool firedSubstitution = false;

    switch (currentAttack) {
        case NarutoAttack::Kick:
            if (attackTimer < 0.12f) {
                velocity = toPlayer * (moveSpeed * 1.35f);
            } else if (attackTimer < 0.32f) {
                velocity = {0.f, 0.f};
                kickActive = true;
                resetContactCooldown(0.f);
            } else if (attackTimer >= attackDuration) {
                finishAttack();
            }
            break;

        case NarutoAttack::Dash:
            velocity = toPlayer * (chaseSpeed * 1.65f);
            if (attackTimer >= attackDuration) {
                finishAttack();
            }
            break;

        case NarutoAttack::CloneSummon:
            velocity = {0.f, 0.f};
            if (attackTimer >= 0.45f && attackTimer - dt < 0.45f) {
                spawnClone(getPosition());
                if (phase == NarutoBossPhase::Three && countActiveClones() < 3) {
                    spawnClone(getPosition());
                }
            }
            if (attackTimer >= attackDuration) {
                finishAttack();
            }
            break;

        case NarutoAttack::OdamaRasengan:
            velocity = {0.f, 0.f};
            if (attackTimer >= 0.75f && attackTimer - dt < 0.75f) {
                spawnOdama(player);
            }
            if (attackTimer >= attackDuration) {
                finishAttack();
            }
            break;

        case NarutoAttack::RasenShuriken:
            velocity = {0.f, 0.f};
            if (attackTimer >= 0.35f && attackTimer - dt < 0.35f) {
                spawnRasenShuriken(player, getPosition() + sf::Vector2f(8.f, 8.f));
            }
            if (attackTimer >= attackDuration) {
                velocity = toPlayer * chaseSpeed;
                finishAttack();
            }
            break;

        case NarutoAttack::Substitution:
            velocity = {0.f, 0.f};
            if (attackTimer >= 0.22f && attackTimer - dt < 0.22f) {
                beginSubstitution(player);
            }
            if (hidden && attackTimer >= 0.5f && attackTimer - dt < 0.5f) {
                hidden = false;
                substitutionDelay = 0.42f;
            }
            if (!hidden && !firedSubstitution) {
                substitutionDelay -= dt;
                if (substitutionDelay <= 0.f) {
                    finishSubstitution(player);
                    firedSubstitution = true;
                }
            }
            if (attackTimer >= attackDuration) {
                hidden = false;
                finishAttack();
            }
            break;

        default:
            if (attackTimer >= attackDuration) {
                finishAttack();
            }
            break;
    }

    applyMovement(dt, map);
}

void NarutoBoss::takeHit(int damage, sf::Vector2f knockback) {
    if (isDead() || deathAnimPending) return;

    Enemy::takeHit(damage, knockback);

    if (health > 0 && !deathAnimPending) {
        kickActive = false;
        hidden = false;
        sf::Vector2f away = knockback;
        if (std::abs(away.x) + std::abs(away.y) < 0.01f) {
            away = {faceLeft ? 1.f : -1.f, 0.f};
        }
        velocity = MathUtils::normalize(away) * (moveSpeed * 1.1f);
        startRecovery(phase == NarutoBossPhase::One ? 1.1f : 0.75f);
    }

    if (deathAnimPending) {
        despawnAllClones();
        EventBus::instance().emit("naruto_boss_defeated");
    }
}

sf::FloatRect NarutoBoss::getContactBounds() const {
    if (hidden || !kickActive) {
        return sf::FloatRect(0.f, 0.f, 0.f, 0.f);
    }

    sf::Vector2f pos = getPosition();
    sf::Vector2f sz = getSize();
    float extend = faceLeft ? -28.f : sz.x;
    return sf::FloatRect(
        pos.x + (faceLeft ? extend : 0.f),
        pos.y + 6.f,
        36.f,
        sz.y - 12.f
    );
}

void NarutoBoss::despawnAllClones() {
    if (!entities) return;

    for (auto& ent : entities->all()) {
        if (auto* clone = dynamic_cast<NarutoCloneEnemy*>(ent.get())) {
            clone->deactivate();
        }
    }
}
