#include "StatisticsScreen.h"

#include "../../save/HighScore.h"
#include "../../utils/AssetPaths.h"

#include <cmath>
#include <sstream>
#include <iomanip>

StatisticsScreen::StatisticsScreen() {
    setBackgroundPath(AssetPaths::getMenuFondoBackground());
}

static std::string formatPlayTime(float seconds) {
    int total = static_cast<int>(seconds);
    int hours = total / 3600;
    int mins = (total % 3600) / 60;
    int secs = total % 60;

    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << hours << ":"
        << std::setw(2) << mins << ":"
        << std::setw(2) << secs;
    return oss.str();
}

static std::string formatLeaderboardLine(int rank, const HighScoreEntry& entry) {
    std::ostringstream oss;
    oss << "#" << rank << " " << entry.name;

    const size_t targetWidth = 18;
    if (entry.name.size() < targetWidth) {
        oss << std::string(targetWidth - entry.name.size(), '.');
    } else {
        oss << " ";
    }

    oss << " " << entry.score;
    return oss.str();
}

void StatisticsScreen::prepare(sf::Vector2u windowSize, const SaveData& data) {
    layoutForWindow(windowSize);

    statLines = {
        "Mejor puntuacion: " + std::to_string(data.bestScore),
        "Record de: " + data.bestScoreHolder,
        "Partidas jugadas: " + std::to_string(data.runsCompleted),
        "Muertes totales: " + std::to_string(data.totalDeaths),
        "Sala maxima alcanzada: " + std::to_string(data.maxRoomReached),
        "Rupias recolectadas: " + std::to_string(data.totalRupeesCollected),
        "Tiempo jugado: " + formatPlayTime(data.totalPlayTimeSeconds),
        "Enemigos derrotados: " + std::to_string(data.enemiesDefeated)
    };

    leaderboardLines = {"TOP 5 AVENTUREROS"};
    for (int i = 0; i < kMaxHighScores; ++i) {
        if (i < static_cast<int>(data.highScores.size())) {
            leaderboardLines.push_back(
                formatLeaderboardLine(i + 1, data.highScores[static_cast<size_t>(i)])
            );
        } else {
            leaderboardLines.push_back("#" + std::to_string(i + 1) + " ---");
        }
    }

    leaderboardStartX = windowSize.x * 0.56f;
}

GameState StatisticsScreen::handleEvent(
    const sf::Event& event,
    sf::RenderWindow& window
) {
    if (event.type == sf::Event::MouseMoved) {
        update(sf::Vector2f(
            static_cast<float>(event.mouseMove.x),
            static_cast<float>(event.mouseMove.y)
        ));
    }

    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mouse(
            static_cast<float>(event.mouseButton.x),
            static_cast<float>(event.mouseButton.y)
        );
        if (handleBackButton(window, mouse, true)) {
            return GameState::MainMenu;
        }
    }

    return GameState::Statistics;
}

void StatisticsScreen::update(sf::Vector2f mousePos) {
    backHovered = backButtonBounds.contains(mousePos);
}

void StatisticsScreen::draw(sf::RenderWindow& window) const {
    drawBackground(window);
    drawTitle(window, "Estadisticas", 80.f);
    drawLines(window, statLines, 160.f, 26, 120.f);
    drawLines(window, leaderboardLines, 160.f, 26, leaderboardStartX);
    drawBackButton(window, backHovered);
}
