#include "StatisticsScreen.h"

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

void StatisticsScreen::prepare(sf::Vector2u windowSize, const SaveData& data) {
    layoutForWindow(windowSize);

    statLines = {
        "Mejor puntuacion: " + std::to_string(data.bestScore),
        "Partidas jugadas: " + std::to_string(data.runsCompleted),
        "Muertes totales: " + std::to_string(data.totalDeaths),
        "Sala maxima alcanzada: " + std::to_string(data.maxRoomReached),
        "Rupias recolectadas: " + std::to_string(data.totalRupeesCollected),
        "Tiempo jugado: " + formatPlayTime(data.totalPlayTimeSeconds),
        "Enemigos derrotados: " + std::to_string(data.enemiesDefeated)
    };
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
    drawLines(window, statLines, 160.f, 26);
    drawBackButton(window, backHovered);
}
