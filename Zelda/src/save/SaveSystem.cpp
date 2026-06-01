#include "SaveSystem.h"
#include "../entity/player/Player.h"

#include <algorithm>
#include <fstream>
#include <sstream>

namespace {

std::string sanitizeStoredName(const std::string& name) {
    std::string clean;
    clean.reserve(name.size());
    for (char c : name) {
        if (c == '\n' || c == '\r') continue;
        clean.push_back(c);
    }
    return clean.empty() ? "Aventurero" : clean;
}

} // namespace

bool SaveSystem::save(const SaveData& data) {
    std::ofstream out(PATH);
    if (!out) return false;

    out << data.metaStats.maxHearts << " "
        << data.metaStats.swordDamage << " "
        << data.metaStats.moveSpeedMult << " "
        << data.runsCompleted << " "
        << data.enemiesDefeated << " "
        << data.spinUnlocked << " "
        << data.shieldUnlocked << " "
        << data.totalDeaths << " "
        << data.maxRoomReached << " "
        << data.totalRupeesCollected << " "
        << data.totalPlayTimeSeconds << " "
        << data.bestScore << "\n";

    out << sanitizeStoredName(data.bestScoreHolder) << "\n";
    out << data.highScores.size() << "\n";
    for (const auto& entry : data.highScores) {
        out << sanitizeStoredName(entry.name) << " " << entry.score << "\n";
    }

    return true;
}

bool SaveSystem::load(SaveData& data) {
    std::ifstream in(PATH);
    if (!in) return false;

    int spin = 0;
    int shield = 0;
    in >> data.metaStats.maxHearts
       >> data.metaStats.swordDamage
       >> data.metaStats.moveSpeedMult
       >> data.runsCompleted
       >> data.enemiesDefeated
       >> spin
       >> shield;

    data.spinUnlocked = spin != 0;
    data.shieldUnlocked = shield != 0;

    if (in >> data.totalDeaths) {
        in >> data.maxRoomReached
           >> data.totalRupeesCollected
           >> data.totalPlayTimeSeconds
           >> data.bestScore;
    }

    std::string line;
    std::getline(in, line);

    if (std::getline(in, line) && !line.empty()) {
        std::istringstream probe(line);
        size_t maybeCount = 0;
        if (probe >> maybeCount && probe.eof()) {
            size_t count = maybeCount;
            data.highScores.clear();
            data.highScores.reserve(count);
            for (size_t i = 0; i < count; ++i) {
                if (!std::getline(in, line)) break;
                std::istringstream row(line);
                std::string name;
                int score = 0;
                row >> name >> score;
                if (name.empty()) continue;
                data.highScores.push_back({sanitizeStoredName(name), score});
            }
        } else {
            data.bestScoreHolder = sanitizeStoredName(line);

            size_t count = 0;
            if (in >> count) {
                std::getline(in, line);
                data.highScores.clear();
                data.highScores.reserve(count);
                for (size_t i = 0; i < count; ++i) {
                    if (!std::getline(in, line)) break;
                    std::istringstream row(line);
                    std::string name;
                    int score = 0;
                    row >> name >> score;
                    if (name.empty()) continue;
                    data.highScores.push_back({sanitizeStoredName(name), score});
                }
            }
        }

        std::sort(data.highScores.begin(), data.highScores.end(),
            [](const HighScoreEntry& a, const HighScoreEntry& b) {
                return a.score > b.score;
            });
        if (data.highScores.size() > kMaxHighScores) {
            data.highScores.resize(kMaxHighScores);
        }
    }

    if (!data.highScores.empty()) {
        data.bestScore = data.highScores.front().score;
        data.bestScoreHolder = data.highScores.front().name;
    }

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
    data.metaStats.maxHearts = player.getPersistedMaxHearts();
}
