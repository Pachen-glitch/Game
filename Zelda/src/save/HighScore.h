#pragma once

#include <algorithm>
#include <string>
#include <vector>

struct HighScoreEntry {
    std::string name;
    int score = 0;
};

constexpr int kMaxHighScores = 5;
constexpr int kMaxPlayerNameLength = 16;

std::string normalizePlayerName(const std::string& raw);

void submitHighScore(
    std::vector<HighScoreEntry>& scores,
    int& bestScore,
    std::string& bestScoreHolder,
    const std::string& name,
    int score
);
