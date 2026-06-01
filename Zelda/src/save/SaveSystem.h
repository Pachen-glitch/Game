#pragma once

#include "../entity/player/PlayerStats.h"

#include <string>

struct SaveData {
    PlayerStats metaStats;
    int runsCompleted = 0;
    int enemiesDefeated = 0;
    int totalDeaths = 0;
    int maxRoomReached = 0;
    int totalRupeesCollected = 0;
    float totalPlayTimeSeconds = 0.f;
    int bestScore = 0;
    bool spinUnlocked = true;
    bool shieldUnlocked = true;
};

class SaveSystem {
public:
    static constexpr const char* PATH = "save.dat";

    bool save(const SaveData& data);
    bool load(SaveData& data);
    void applyToPlayer(class Player& player, const SaveData& data);
    void captureFromPlayer(const class Player& player, SaveData& data);
};
