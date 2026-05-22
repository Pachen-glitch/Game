#include "SaveSystem.h"
#include "../entity/player/Player.h"

#include <algorithm>
#include <fstream>

bool SaveSystem::save(const SaveData& data) {
    std::ofstream out(PATH);
    if (!out) return false;

    out << data.metaStats.maxHearts << " "
        << data.metaStats.swordDamage << " "
        << data.metaStats.moveSpeedMult << " "
        << data.runsCompleted << " "
        << data.enemiesDefeated << " "
        << data.spinUnlocked << " "
        << data.shieldUnlocked;
    return true;
}

bool SaveSystem::load(SaveData& data) {
    std::ifstream in(PATH);
    if (!in) return false;

    int spin, shield;
    in >> data.metaStats.maxHearts
       >> data.metaStats.swordDamage
       >> data.metaStats.moveSpeedMult
       >> data.runsCompleted
       >> data.enemiesDefeated
       >> spin >> shield;
    data.spinUnlocked = spin != 0;
    data.shieldUnlocked = shield != 0;
    return true;
}

void SaveSystem::applyToPlayer(Player& player, const SaveData& data) {
    player.getStats().swordDamage = std::max(
        player.getStats().swordDamage, data.metaStats.swordDamage
    );
    player.getStats().moveSpeedMult = std::max(
        player.getStats().moveSpeedMult, data.metaStats.moveSpeedMult
    );
    player.getStats().hasSpinAttack = data.spinUnlocked;
    player.getStats().hasShield = data.shieldUnlocked;
}

void SaveSystem::captureFromPlayer(const Player& player, SaveData& data) {
    data.metaStats = player.getStats();
}
