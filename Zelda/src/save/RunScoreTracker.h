#pragma once

#include "../entity/enemy/EnemyState.h"

#include <unordered_set>

// Tracks score for the active run. Persisted only when the run ends.
class RunScoreTracker {
public:
    int currentRunScore = 0;
    float survivalSeconds = 0.f;

    void reset();

    void addScore(int points, const char* label);
    void addSurvivalTime(float seconds);

    void onRupeeCollected(int value);
    void onEnemyDefeated(EnemyKind kind);
    void onHeartCollected();
    void onKeyCollected();
    void onChestOpened();
    void onShopPurchase(int cost);
    void onRoomFirstEnter(int roomId);
    void onRoomCleared(int roomId);
    void onBossGateUnlocked();
    void onNarutoBossDefeated();

    int finalizeScore();

    static RunScoreTracker* active();
    static void setActive(RunScoreTracker* tracker);

private:
    std::unordered_set<int> visitedRooms_;
    std::unordered_set<int> clearedRooms_;

    static RunScoreTracker* active_;
};
