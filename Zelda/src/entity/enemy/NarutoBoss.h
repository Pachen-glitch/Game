#pragma once

#include "Enemy.h"
#include "NarutoAttack.h"

class EntityManager;
class Player;

// Final boss — phased AI with attack memory, clones, and projectiles.
class NarutoBoss : public Enemy {
public:
    explicit NarutoBoss(sf::Vector2f pos);

    void think(Player& player, float dt, const Map& map) override;
    void takeHit(int damage, sf::Vector2f knockback) override;
    sf::FloatRect getContactBounds() const override;

    void setEntityManager(EntityManager* mgr) { entities = mgr; }

    NarutoBossPhase getPhase() const { return phase; }
    NarutoAttack getCurrentAttack() const { return currentAttack; }
    bool facesLeft() const { return faceLeft; }
    bool isHidden() const { return hidden; }
    bool isIntroActive() const { return introTimer > 0.f; }
    bool isKickActive() const { return kickActive; }

    void despawnAllClones();

private:
    enum class Mode {
        Intro,
        Recovery,
        Chase,
        Executing
    };

    void updatePhaseFromHealth();
    void updateIntro(float dt, const Map& map);
    void updateRecovery(float dt, const Map& map);
    void updateChaseMode(Player& player, float dt, const Map& map);
    void updateExecuting(Player& player, float dt, const Map& map);

    void startRecovery(float duration);
    void startAttack(NarutoAttack attack);
    void finishAttack();

    NarutoAttack pickNextAttack(const Player& player);
    bool isAttackReady(NarutoAttack attack) const;
    void stampAttackCooldown(NarutoAttack attack);
    void registerAttackUse(NarutoAttack attack);

    int countActiveClones() const;
    void spawnClone(sf::Vector2f nearPos);
    void spawnOdama(const Player& player);
    void spawnRasenShuriken(const Player& player, sf::Vector2f origin);
    void beginSubstitution(const Player& player);
    void finishSubstitution(const Player& player);

    void updateFacing(const Player& player);
    void tickCooldowns(float dt);

    EntityManager* entities = nullptr;

    NarutoBossPhase phase = NarutoBossPhase::One;
    NarutoAttack currentAttack = NarutoAttack::None;
    NarutoAttack lastAttack = NarutoAttack::None;
    int sameAttackStreak = 0;

    Mode mode = Mode::Intro;

    float introTimer = 1.8f;
    float recoveryTimer = 0.f;
    float attackTimer = 0.f;
    float attackDuration = 0.f;
    bool hidden = false;
    float substitutionDelay = 0.f;

    bool kickActive = false;
    bool musicStarted = false;
    bool battleMusicStarted = false;

    float odamaCooldown = 0.f;
    float rasenCooldown = 0.f;
    float cloneCooldown = 0.f;
    float substitutionCooldown = 0.f;
    float dashCooldown = 0.f;

    bool faceLeft = true;
};
