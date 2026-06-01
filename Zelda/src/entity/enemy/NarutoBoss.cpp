#include "NarutoBoss.h"



#include "NarutoCloneEnemy.h"

#include "NarutoProjectile.h"

#include "../base/EntityManager.h"

#include "../player/Player.h"

#include "../../audio/AudioManager.h"

#include "../../core/CombatFeel.h"

#include "../../interaction/EventBus.h"

#include "../../map/Map.h"

#include "../../utils/AssetPaths.h"

#include "../../utils/MathUtils.h"



#include <algorithm>

#include <cmath>

#include <cstdlib>



namespace {



float rand01() {

    return static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);

}



constexpr float kTooClose = 44.f;

constexpr float kCloseMax = 76.f;

constexpr float kMediumMin = 95.f;

constexpr float kMediumMax = 200.f;

constexpr float kLongMin = 155.f;

constexpr float kRasenMinDist = 105.f;

constexpr float kMeleeBackOffDist = 28.f;

constexpr float kMeleeHoldDist = 40.f;

constexpr float kMeleeStopDist = 52.f;

constexpr float kDefensiveTeleportDist = 185.f;

constexpr float kRasenCastFrameDuration = 0.18f;

constexpr float kRasenCastDuration = kRasenCastFrameDuration * 4.f;

constexpr float kOdamaCastFrameDuration = 0.18f;

constexpr float kOdamaCastDuration = kOdamaCastFrameDuration * 7.f;

constexpr float kOdamaCloseCastDuration = kOdamaCastFrameDuration * 10.f;

constexpr float kJutsuAttackTail = 0.12f;

constexpr float kRasenProjectileFrame = kRasenCastDuration;

constexpr float kComboHitWindow = 2.2f;

constexpr int kComboHitThreshold = 2;

constexpr float kSubstitutionSmokeDuration = 0.5f;

constexpr float kDecideMin = 1.f;

constexpr float kDecideMax = 2.f;

constexpr float kCloneRollChance = 0.24f;

constexpr float kRecoveryPursuitScale = 0.82f;



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

    hidden = true;

    introSmokeTimer = 0.56f;

    introTimer = 1.0f;

    activeOdamaSpawnFrame = kOdamaCastDuration;

    resetDecideTimer();

}



NarutoBoss::RangeBand NarutoBoss::getRangeBand(float dist) const {

    if (dist < kTooClose) return RangeBand::TooClose;

    if (dist <= kCloseMax) return RangeBand::Close;

    if (dist >= kLongMin) return RangeBand::Long;

    if (dist >= kMediumMin && dist <= kMediumMax) return RangeBand::Medium;

    if (dist > kCloseMax && dist < kMediumMin) return RangeBand::Medium;

    return RangeBand::Close;

}



void NarutoBoss::resetDecideTimer() {

    decideTimer = kDecideMin + rand01() * (kDecideMax - kDecideMin);

}



void NarutoBoss::updatePhaseFromHealth() {

    NarutoBossPhase next = NarutoBossPhase::One;

    float ratio = static_cast<float>(health) / static_cast<float>(maxHealth);

    if (ratio > 0.7f) {

        next = NarutoBossPhase::One;

    } else if (ratio > 0.3f) {

        next = NarutoBossPhase::Two;

    } else {

        next = NarutoBossPhase::Three;

    }



    if (next != phase && mode != Mode::Intro) {

        if (next == NarutoBossPhase::Two && !announcedPhaseTwo) {

            triggerPhaseTransition(NarutoBossPhase::Two);

        } else if (next == NarutoBossPhase::Three && !announcedPhaseThree) {

            triggerPhaseTransition(NarutoBossPhase::Three);

        }

    }



    phase = next;

}



void NarutoBoss::triggerPhaseTransition(NarutoBossPhase newPhase) {

    if (newPhase == NarutoBossPhase::Two) {

        announcedPhaseTwo = true;

    } else if (newPhase == NarutoBossPhase::Three) {

        announcedPhaseThree = true;

    }



    chakraBurstTimer = 0.75f;

    CombatFeel::instance().triggerHitPause(0.14f);

    CombatFeel::instance().triggerScreenShake(

        newPhase == NarutoBossPhase::Three ? 14.f : 9.f,

        0.45f

    );

    CombatFeel::instance().setEnemyFlashTimer(0.5f);

    AudioManager::instance().playNarutoPhaseTransitionSound();

    EventBus::instance().emit("naruto_phase_transition");

}



void NarutoBoss::tickCooldowns(float dt) {

    odamaCooldown = std::max(0.f, odamaCooldown - dt);

    rasenCooldown = std::max(0.f, rasenCooldown - dt);

    cloneCooldown = std::max(0.f, cloneCooldown - dt);

    substitutionCooldown = std::max(0.f, substitutionCooldown - dt);

    dashCooldown = std::max(0.f, dashCooldown - dt);

    defensiveSubCooldown = std::max(0.f, defensiveSubCooldown - dt);



    if (chakraBurstTimer > 0.f) {

        chakraBurstTimer = std::max(0.f, chakraBurstTimer - dt);

    }



    if (consecutiveHitWindow > 0.f) {

        consecutiveHitWindow -= dt;

        if (consecutiveHitWindow <= 0.f) {

            consecutiveHits = 0;

        }

    }

}



void NarutoBoss::trackComboHit() {

    ++consecutiveHits;

    consecutiveHitWindow = kComboHitWindow;

}



void NarutoBoss::tryAntiComboResponse() {

    if (consecutiveHits < kComboHitThreshold) return;

    if (defensiveSubCooldown > 0.f) return;

    if (mode == Mode::Intro || deathAnimPending) return;



    consecutiveHits = 0;

    consecutiveHitWindow = 0.f;

    kickActive = false;

    hidden = false;

    hurtAnimTimer.reset();

    startAttack(NarutoAttack::DefensiveSubstitution);

}



void NarutoBoss::updateFacing(const Player& player) {

    faceLeft = player.getPosition().x < getPosition().x;

    facingDirection = faceLeft ? Direction::LEFT : Direction::RIGHT;

}



void NarutoBoss::updatePursuit(const Player& player) {

    float dist = MathUtils::distance(getPosition(), player.getPosition());

    sf::Vector2f toPlayer = MathUtils::directionTo(getPosition(), player.getPosition());

    sf::Vector2f awayFromPlayer = MathUtils::directionTo(

        player.getPosition(), getPosition()

    );



    if (dist < kMeleeBackOffDist) {

        velocity = awayFromPlayer * (moveSpeed * 0.9f);

    } else if (dist <= kMeleeStopDist) {

        velocity = {0.f, 0.f};

    } else if (dist <= kMeleeHoldDist + 18.f) {

        velocity = toPlayer * (chaseSpeed * 0.55f);

    } else {

        velocity = toPlayer * chaseSpeed;

    }



    currentAttack = NarutoAttack::None;

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



    if (hurtAnimTimer.isActive() && mode != Mode::Executing) {

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

            updateRecovery(player, dt, map);

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



    if (introSmokeTimer > 0.f) {

        introSmokeTimer -= dt;

        hidden = true;

        if (introSmokeTimer <= 0.f) {

            hidden = false;

            if (!battleMusicStarted) {

                battleMusicStarted = true;

                AudioManager::instance().playBossBattleMusic();

            }

        }

    }



    introTimer -= dt;



    if (introTimer <= 0.f) {

        hidden = false;

        mode = Mode::Chase;

        resetDecideTimer();

    }



    applyMovement(dt, map);

}



void NarutoBoss::startRecovery(float duration) {

    mode = Mode::Recovery;

    recoveryTimer = duration;

    currentAttack = NarutoAttack::None;

    kickActive = false;

}



void NarutoBoss::updateRecovery(Player& player, float dt, const Map& map) {

    recoveryTimer -= dt;

    updatePursuit(player);

    velocity *= kRecoveryPursuitScale;



    if (recoveryTimer <= 0.f) {

        mode = Mode::Chase;

        resetDecideTimer();

    }



    applyMovement(dt, map);

}



float NarutoBoss::odamaSpawnFrameFor(float dist) const {

    if (dist < kMediumMin) {

        return kOdamaCloseCastDuration;

    }

    return kOdamaCastDuration;

}



void NarutoBoss::configureOdamaCast(float distToPlayer) {

    activeOdamaSpawnFrame = odamaSpawnFrameFor(distToPlayer);

    attackDuration = activeOdamaSpawnFrame + kJutsuAttackTail;

}



float NarutoBoss::attackDurationFor(NarutoAttack attack) const {

    switch (attack) {

        case NarutoAttack::Kick:

            return 0.55f;

        case NarutoAttack::Dash:

            return 0.35f;

        case NarutoAttack::CloneSummon:

            return 1.05f;

        case NarutoAttack::OdamaRasengan:

            return kOdamaCastDuration + kJutsuAttackTail;

        case NarutoAttack::RasenShuriken:

            return kRasenCastDuration + kJutsuAttackTail;

        case NarutoAttack::Substitution:

        case NarutoAttack::DefensiveSubstitution:

            return kSubstitutionSmokeDuration * 2.f + 0.05f;

        default:

            return 0.5f;

    }

}



void NarutoBoss::playCastSoundFor(NarutoAttack attack) const {

    if (attack == NarutoAttack::OdamaRasengan) {

        AudioManager::instance().playOdamaSound();

    } else if (attack == NarutoAttack::RasenShuriken) {

        AudioManager::instance().playRasenShurikenSound();

    }

}



void NarutoBoss::updateChaseMode(Player& player, float dt, const Map& map) {

    float dist = MathUtils::distance(getPosition(), player.getPosition());



    updatePursuit(player);



    decideTimer -= dt;

    if (decideTimer <= 0.f) {

        resetDecideTimer();



        NarutoAttack choice = pickReactiveAttack(player, dist);

        if (choice != NarutoAttack::None) {

            startAttack(choice, &player);

        }

    }



    applyMovement(dt, map);

}



bool NarutoBoss::isAttackReady(NarutoAttack attack, float dist) const {

    if (attack == lastAttack && sameAttackStreak >= 2) {

        return false;

    }



    RangeBand band = getRangeBand(dist);



    switch (attack) {

        case NarutoAttack::Kick:

            return band == RangeBand::Close || band == RangeBand::TooClose;

        case NarutoAttack::Dash:

            return dashCooldown <= 0.f &&

                   (band == RangeBand::Close || band == RangeBand::TooClose);

        case NarutoAttack::CloneSummon:

            return phase != NarutoBossPhase::One &&

                   cloneCooldown <= 0.f &&

                   countActiveClones() < 3;

        case NarutoAttack::OdamaRasengan:

            return phase != NarutoBossPhase::One && odamaCooldown <= 0.f;

        case NarutoAttack::RasenShuriken:

            return phase != NarutoBossPhase::One &&

                   rasenCooldown <= 0.f &&

                   band != RangeBand::TooClose &&

                   dist >= kRasenMinDist;

        case NarutoAttack::Substitution:

            return phase != NarutoBossPhase::One &&

                   substitutionCooldown <= 0.f &&

                   band != RangeBand::TooClose;

        case NarutoAttack::DefensiveSubstitution:

            return defensiveSubCooldown <= 0.f;

        default:

            return false;

    }

}



NarutoAttack NarutoBoss::pickReactiveAttack(

    const Player& player,

    float dist

) const {

    (void)player;

    RangeBand band = getRangeBand(dist);



    bool underPressure =

        band == RangeBand::TooClose ||

        consecutiveHits >= 1 ||

        dist < kCloseMax + 18.f;



    if (underPressure && phase != NarutoBossPhase::One) {

        if (isAttackReady(NarutoAttack::DefensiveSubstitution, dist)) {

            return NarutoAttack::DefensiveSubstitution;

        }

        if (isAttackReady(NarutoAttack::Substitution, dist)) {

            return NarutoAttack::Substitution;

        }

    }



    if (phase != NarutoBossPhase::One &&

        isAttackReady(NarutoAttack::CloneSummon, dist) &&

        rand01() < kCloneRollChance) {

        return NarutoAttack::CloneSummon;

    }



    if (band == RangeBand::Close || band == RangeBand::TooClose) {

        if (isAttackReady(NarutoAttack::Kick, dist)) {

            return NarutoAttack::Kick;

        }

    }



    if (band == RangeBand::Medium ||

        (dist >= kMediumMin && dist < kLongMin)) {

        if (isAttackReady(NarutoAttack::OdamaRasengan, dist)) {

            return NarutoAttack::OdamaRasengan;

        }

    }



    if (band == RangeBand::Long || dist >= kRasenMinDist) {

        if (isAttackReady(NarutoAttack::RasenShuriken, dist)) {

            return NarutoAttack::RasenShuriken;

        }

    }



    if (isAttackReady(NarutoAttack::Kick, dist)) {

        return NarutoAttack::Kick;

    }

    if (isAttackReady(NarutoAttack::OdamaRasengan, dist)) {

        return NarutoAttack::OdamaRasengan;

    }

    if (isAttackReady(NarutoAttack::RasenShuriken, dist)) {

        return NarutoAttack::RasenShuriken;

    }



    return NarutoAttack::None;

}



NarutoAttack NarutoBoss::pickSubstitutionFollowUp() const {

    return (rand01() < 0.5f)

        ? NarutoAttack::OdamaRasengan

        : NarutoAttack::RasenShuriken;

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

            cloneCooldown = phase == NarutoBossPhase::Two ? 4.5f : 3.f;

            break;

        case NarutoAttack::OdamaRasengan:

            odamaCooldown = phase == NarutoBossPhase::Three ? 4.f :

                            phase == NarutoBossPhase::Two ? 6.f : 10.f;

            break;

        case NarutoAttack::RasenShuriken:

            rasenCooldown = 4.f;

            break;

        case NarutoAttack::Substitution:

            substitutionCooldown = 8.f;

            break;

        case NarutoAttack::DefensiveSubstitution:

            defensiveSubCooldown = 6.f;

            break;

        default:

            break;

    }

}



void NarutoBoss::startAttack(NarutoAttack attack, const Player* player) {

    registerAttackUse(attack);

    stampAttackCooldown(attack);

    playCastSoundFor(attack);



    mode = Mode::Executing;

    currentAttack = attack;

    attackTimer = 0.f;

    kickActive = false;

    velocity = {0.f, 0.f};

    projectileSpawnedThisAttack = false;

    substitutionTeleported = false;

    substitutionSmokeDone = false;

    subReappearSmoke = false;

    attackDuration = attackDurationFor(attack);



    if (attack == NarutoAttack::OdamaRasengan && player) {

        float dist = MathUtils::distance(getPosition(), player->getPosition());

        configureOdamaCast(dist);

    } else if (attack == NarutoAttack::OdamaRasengan) {

        activeOdamaSpawnFrame = kOdamaCastDuration;

    }



    if (attack == NarutoAttack::Substitution ||

        attack == NarutoAttack::DefensiveSubstitution) {

        hidden = false;

    }

}



void NarutoBoss::finishAttack() {

    kickActive = false;

    hidden = false;

    subReappearSmoke = false;

    currentAttack = NarutoAttack::None;

    mode = Mode::Chase;

    decideTimer = std::min(decideTimer, 0.4f);

}



void NarutoBoss::spawnClone(sf::Vector2f nearPos) {

    if (!entities || countActiveClones() >= 3) return;



    static const sf::Vector2f offsets[] = {

        {48.f, 0.f}, {-48.f, 0.f}, {0.f, 48.f}, {0.f, -48.f}

    };



    int idx = static_cast<int>(rand01() * 4.f) % 4;

    entities->spawn<NarutoCloneEnemy>(nearPos + offsets[idx]);



    if (phase == NarutoBossPhase::Three && countActiveClones() < 3 && rand01() < 0.6f) {

        idx = (idx + 1) % 4;

        entities->spawn<NarutoCloneEnemy>(nearPos + offsets[idx]);

    }

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

    if (!entities || projectileSpawnedThisAttack) return;



    sf::Vector2f bossPos = getPosition();

    sf::Vector2f dir = MathUtils::directionTo(bossPos, player.getPosition());

    sf::Vector2f origin = bossPos + dir * 40.f + sf::Vector2f(8.f, 8.f);



    entities->spawn<NarutoProjectile>(

        origin,

        dir * 95.f,

        NarutoProjectileType::Odama,

        3.f

    );

    projectileSpawnedThisAttack = true;

}



void NarutoBoss::spawnRasenShuriken(const Player& player) {

    if (!entities || projectileSpawnedThisAttack) return;



    sf::Vector2f bossPos = getPosition();

    sf::Vector2f origin = bossPos + sf::Vector2f(8.f, 8.f);

    sf::Vector2f dir = MathUtils::directionTo(origin, player.getPosition());



    entities->spawn<NarutoProjectile>(

        origin,

        dir * 210.f,

        NarutoProjectileType::RasenShuriken,

        2.f

    );

    projectileSpawnedThisAttack = true;

}



void NarutoBoss::startSubstitutionFollowUp(

    NarutoAttack followUp,

    const Player& player

) {

    hidden = false;

    updateFacing(player);

    registerAttackUse(followUp);

    stampAttackCooldown(followUp);

    lastAttack = followUp;

    currentAttack = followUp;

    attackTimer = 0.f;

    projectileSpawnedThisAttack = false;

    velocity = {0.f, 0.f};

    attackDuration = attackDurationFor(followUp);



    switch (followUp) {

        case NarutoAttack::RasenShuriken:

            playCastSoundFor(followUp);

            break;

        case NarutoAttack::OdamaRasengan:

            configureOdamaCast(

                MathUtils::distance(getPosition(), player.getPosition())

            );

            playCastSoundFor(followUp);

            break;

        default:

            attackDuration = 0.75f;

            break;

    }

}



void NarutoBoss::beginOffensiveSubstitution(const Player& player) {

    sf::Vector2f toBoss = MathUtils::directionTo(

        player.getPosition(), getPosition()

    );

    setPosition(player.getPosition() + toBoss * kMeleeStopDist);

    hidden = true;

    velocity = {0.f, 0.f};

    updateFacing(player);

}



void NarutoBoss::completeSubstitutionSmoke(Player& player, bool offensive) {

    hidden = false;

    subReappearSmoke = false;

    substitutionSmokeDone = true;



    if (offensive) {

        finishOffensiveSubstitution(player);

    } else {

        finishDefensiveSubstitution(player);

    }

}



void NarutoBoss::updateSubstitutionExecuting(

    Player& player,

    float dt,

    const Map& map,

    bool offensive

) {

    velocity = {0.f, 0.f};



    if (attackTimer < kSubstitutionSmokeDuration) {

        hidden = false;

        subReappearSmoke = false;

    } else if (!substitutionTeleported) {

        if (offensive) {

            beginOffensiveSubstitution(player);

        } else {

            beginDefensiveSubstitution(player);

        }

        substitutionTeleported = true;

        hidden = true;

        subReappearSmoke = true;

    } else if (attackTimer < kSubstitutionSmokeDuration * 2.f) {

        hidden = true;

        subReappearSmoke = true;

    } else if (!substitutionSmokeDone) {

        completeSubstitutionSmoke(player, offensive);

    }



    applyMovement(dt, map);

}



void NarutoBoss::finishOffensiveSubstitution(const Player& player) {

    startSubstitutionFollowUp(pickSubstitutionFollowUp(), player);

}



void NarutoBoss::beginDefensiveSubstitution(const Player& player) {

    sf::Vector2f away = MathUtils::directionTo(

        player.getPosition(), getPosition()

    );

    setPosition(getPosition() + away * kDefensiveTeleportDist);

    hidden = true;

    velocity = {0.f, 0.f};

    updateFacing(player);

}



void NarutoBoss::finishDefensiveSubstitution(const Player& player) {

    startSubstitutionFollowUp(pickSubstitutionFollowUp(), player);

}



void NarutoBoss::updateExecuting(Player& player, float dt, const Map& map) {

    attackTimer += dt;

    sf::Vector2f toPlayer = MathUtils::directionTo(getPosition(), player.getPosition());

    switch (currentAttack) {

        case NarutoAttack::Kick:
            if (attackTimer < 0.10f) {
                velocity = toPlayer * (moveSpeed * 1.25f);
            } else if (attackTimer < 0.44f) {
                if (attackTimer < 0.18f) {
                    velocity = toPlayer * (moveSpeed * 0.55f);
                } else {
                    velocity = {0.f, 0.f};
                }
                if (!kickActive) {
                    kickActive = true;
                    contactCooldown.reset();
                }
            } else {
                kickActive = false;
                velocity = {0.f, 0.f};
                if (attackTimer >= attackDuration) {
                    finishAttack();
                }
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

            if (attackTimer >= 0.58f && attackTimer - dt < 0.58f) {

                spawnClone(getPosition());

            }

            if (attackTimer >= attackDuration) {

                finishAttack();

            }

            break;



        case NarutoAttack::OdamaRasengan:

            velocity = {0.f, 0.f};

            if (attackTimer >= activeOdamaSpawnFrame &&

                attackTimer - dt < activeOdamaSpawnFrame) {

                spawnOdama(player);

            }

            if (attackTimer >= attackDuration) {

                finishAttack();

            }

            break;



        case NarutoAttack::RasenShuriken:

            velocity = {0.f, 0.f};

            if (attackTimer >= kRasenProjectileFrame &&

                attackTimer - dt < kRasenProjectileFrame) {

                spawnRasenShuriken(player);

            }

            if (attackTimer >= attackDuration) {

                finishAttack();

            }

            break;



        case NarutoAttack::Substitution:

            updateSubstitutionExecuting(player, dt, map, true);

            return;



        case NarutoAttack::DefensiveSubstitution:

            updateSubstitutionExecuting(player, dt, map, false);

            return;



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

    if (isIntroSmokeActive() || isIntroActive()) return;



    trackComboHit();



    Enemy::takeHit(damage, knockback);



    if (health > 0 && !deathAnimPending) {

        tryAntiComboResponse();

        if (mode == Mode::Executing &&

            currentAttack == NarutoAttack::DefensiveSubstitution) {

            return;

        }



        kickActive = false;

        hidden = false;

        sf::Vector2f away = knockback;

        if (std::abs(away.x) + std::abs(away.y) < 0.01f) {

            away = {faceLeft ? 1.f : -1.f, 0.f};

        }

        velocity = MathUtils::normalize(away) * (moveSpeed * 1.1f);

        startRecovery(phase == NarutoBossPhase::One ? 0.45f : 0.3f);

        decideTimer = std::min(decideTimer, 0.25f);

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
    constexpr float kKickReach = 44.f;
    constexpr float kKickBodyOverlap = 16.f;
    const float y = pos.y + 6.f;
    const float h = sz.y - 12.f;

    if (faceLeft) {
        return sf::FloatRect(
            pos.x - kKickReach,
            y,
            kKickReach + kKickBodyOverlap,
            h
        );
    }

    return sf::FloatRect(
        pos.x + sz.x - kKickBodyOverlap,
        y,
        kKickReach + kKickBodyOverlap,
        h
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

