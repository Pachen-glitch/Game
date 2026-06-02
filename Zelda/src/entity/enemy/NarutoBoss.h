#pragma once



#include "Enemy.h"

#include "NarutoAttack.h"



class EntityManager;

class Player;



// Final boss — phased, reactive behavior AI.

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

    bool isIntroSmokeActive() const { return introSmokeTimer > 0.f; }

    bool isInIntroMode() const { return mode == Mode::Intro; }

    bool isSubstitutionReappearSmoke() const { return subReappearSmoke; }

    bool isKickActive() const { return kickActive; }

    float getChakraBurstTimer() const { return chakraBurstTimer; }



    void despawnAllClones();



private:

    enum class Mode {

        Intro,

        Recovery,

        Chase,

        Executing

    };



    enum class RangeBand {

        TooClose,

        Close,

        Medium,

        Long

    };



    void updatePhaseFromHealth();

    void triggerPhaseTransition(NarutoBossPhase newPhase);

    void updateIntro(float dt, const Map& map);

    void updateRecovery(Player& player, float dt, const Map& map);

    void updateChaseMode(Player& player, float dt, const Map& map);

    void updateExecuting(Player& player, float dt, const Map& map);

    void updateSubstitutionExecuting(Player& player, float dt, const Map& map, bool offensive);



    void startRecovery(float duration);

    void startAttack(NarutoAttack attack, const Player* player = nullptr);

    void finishAttack();

    void completeSubstitutionSmoke(Player& player, bool offensive);



    RangeBand getRangeBand(float dist) const;

    bool isAttackReady(NarutoAttack attack, float dist) const;

    NarutoAttack pickReactiveAttack(const Player& player, float dist) const;

    NarutoAttack pickSubstitutionFollowUp() const;

    void updatePursuit(const Player& player);

    void resetDecideTimer();

    float odamaSpawnFrameFor(float dist) const;

    float attackDurationFor(NarutoAttack attack) const;

    void playCastSoundFor(NarutoAttack attack) const;

    void configureOdamaCast(float distToPlayer);



    void stampAttackCooldown(NarutoAttack attack);

    void registerAttackUse(NarutoAttack attack);



    int countActiveClones() const;

    void spawnClone(sf::Vector2f nearPos);

    void spawnOdama(const Player& player);

    void spawnRasenShuriken(const Player& player);

    void beginOffensiveSubstitution(const Player& player);

    void finishOffensiveSubstitution(const Player& player);

    void beginDefensiveSubstitution(const Player& player);

    void finishDefensiveSubstitution(const Player& player);

    void startSubstitutionFollowUp(NarutoAttack followUp, const Player& player);



    void updateFacing(const Player& player);

    void tickCooldowns(float dt);

    void trackComboHit();

    void tryAntiComboResponse();



    EntityManager* entities = nullptr;



    NarutoBossPhase phase = NarutoBossPhase::One;

    NarutoAttack currentAttack = NarutoAttack::None;

    NarutoAttack lastAttack = NarutoAttack::None;

    int sameAttackStreak = 0;



    Mode mode = Mode::Intro;



    float introTimer = 1.0f;

    float introSmokeTimer = 0.56f;

    float recoveryTimer = 0.f;

    float attackTimer = 0.f;

    float attackDuration = 0.f;

    float activeOdamaSpawnFrame = 0.f;

    float decideTimer = 0.f;

    bool hidden = false;

    float chakraBurstTimer = 0.f;



    bool kickActive = false;

    bool musicStarted = false;

    bool battleMusicStarted = false;

    bool announcedPhaseTwo = false;

    bool announcedPhaseThree = false;



    int consecutiveHits = 0;

    float consecutiveHitWindow = 0.f;



    float odamaCooldown = 0.f;

    float rasenCooldown = 0.f;

    float cloneCooldown = 0.f;

    float substitutionCooldown = 0.f;

    float dashCooldown = 0.f;

    float defensiveSubCooldown = 0.f;// Cooldowns for each special attack, managed by stamping the current time when used and checking against that.



    bool faceLeft = true;

    bool projectileSpawnedThisAttack = false;

    bool substitutionTeleported = false;

    bool substitutionSmokeDone = false;

    bool subReappearSmoke = false;

};

