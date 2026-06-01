#include "HighScore.h"

#include <cctype>
// Normaliza el nombre del jugador
std::string normalizePlayerName(const std::string& raw) {
    std::string trimmed;
    trimmed.reserve(kMaxPlayerNameLength);
// Recorre el nombre del jugador
    for (char c : raw) {
        if (c == '\n' || c == '\r' || c == '\t') continue;
        if (static_cast<unsigned char>(c) < 32) continue;
        trimmed.push_back(c);
        if (trimmed.size() >= static_cast<size_t>(kMaxPlayerNameLength)) break;
    }

    while (!trimmed.empty() && std::isspace(static_cast<unsigned char>(trimmed.back()))) {
        trimmed.pop_back();
    }

    size_t start = 0;// Inicia el indice del nombre del jugador
    while (start < trimmed.size() &&
           std::isspace(static_cast<unsigned char>(trimmed[start]))) {
        ++start;
    }
    if (start > 0) trimmed = trimmed.substr(start);

    return trimmed.empty() ? "Aventurero" : trimmed;
}

void submitHighScore(// Envia el puntaje del jugador
    std::vector<HighScoreEntry>& scores,
    int& bestScore,
    std::string& bestScoreHolder,
    const std::string& name,
    int score
) {
    const std::string entryName = normalizePlayerName(name);

    scores.push_back({entryName, score});

    std::sort(scores.begin(), scores.end(),
        [](const HighScoreEntry& a, const HighScoreEntry& b) {
            if (a.score != b.score) return a.score > b.score;
            return a.name < b.name;
        });

    if (static_cast<int>(scores.size()) > kMaxHighScores) {
        scores.resize(kMaxHighScores);
    }

    if (!scores.empty()) {
        bestScore = scores.front().score;
        bestScoreHolder = scores.front().name;
    } else if (score > bestScore) {
        bestScore = score;
        bestScoreHolder = entryName;
    }
}
