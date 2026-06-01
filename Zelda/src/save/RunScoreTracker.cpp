#include "RunScoreTracker.h"

#include <iostream>

RunScoreTracker* RunScoreTracker::active_ = nullptr;

void RunScoreTracker::reset() {
    currentRunScore = 0;
    survivalSeconds = 0.f;
    visitedRooms_.clear();
    clearedRooms_.clear();
}

RunScoreTracker* RunScoreTracker::active() {
    return active_;
}

void RunScoreTracker::setActive(RunScoreTracker* tracker) {
    active_ = tracker;
}

void RunScoreTracker::addScore(int points, const char* label) {
    if (points <= 0) return;
    currentRunScore += points;

#ifndef NDEBUG
    if (label) {
        std::cerr << "[Score] +" << points << " " << label << "\n";
    }
#endif
}

void RunScoreTracker::addSurvivalTime(float seconds) {
    if (seconds > 0.f) {
        survivalSeconds += seconds;
    }
}

void RunScoreTracker::onRupeeCollected(int value) {
    addScore(value, "Rupee");
}

void RunScoreTracker::onEnemyDefeated(EnemyKind kind) {
    switch (kind) {
        case EnemyKind::Slime:
            addScore(50, "Slime");
            break;
        case EnemyKind::Skeleton:
            addScore(150, "Skeleton");
            break;
        case EnemyKind::Summoner:
            addScore(250, "Summoner");
            break;
        case EnemyKind::Naruto:
            break;
        default:
            break;
    }
}

void RunScoreTracker::onHeartCollected() {
    addScore(50, "Heart");
}

void RunScoreTracker::onKeyCollected() {
    addScore(100, "Key");
}

void RunScoreTracker::onChestOpened() {
    addScore(100, "Chest");
}

void RunScoreTracker::onShopPurchase(int cost) {
    addScore(cost, "Shop");
}

void RunScoreTracker::onRoomFirstEnter(int roomId) {
    if (visitedRooms_.count(roomId) > 0) return;
    visitedRooms_.insert(roomId);
    addScore(25, "Room");
}

void RunScoreTracker::onRoomCleared(int roomId) {
    if (clearedRooms_.count(roomId) > 0) return;
    clearedRooms_.insert(roomId);
    addScore(100, "RoomClear");
}

void RunScoreTracker::onBossGateUnlocked() {
    addScore(500, "BossGate");
}

void RunScoreTracker::onNarutoBossDefeated() {
    addScore(5000, "NarutoBoss");
}

int RunScoreTracker::finalizeScore() {
    const int minutes = static_cast<int>(survivalSeconds / 60.f);
    const int survivalBonus = minutes * 12;
    if (survivalBonus > 0) {
        addScore(survivalBonus, "Survival");
    }
    return currentRunScore;
}
