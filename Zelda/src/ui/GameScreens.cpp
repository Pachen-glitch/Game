#include "GameScreens.h"
#include "../utils/FontLoader.h"

#include <SFML/Graphics/Text.hpp>

namespace {

void drawCenteredText(
    sf::RenderWindow& window,
    const sf::Font& font,
    const std::string& msg,
    float y,
    unsigned size,
    sf::Color color
) {
    sf::Text t;
    t.setFont(font);
    t.setString(msg);
    t.setCharacterSize(size);
    t.setFillColor(color);
    sf::FloatRect b = t.getLocalBounds();
    t.setOrigin(b.width / 2.f, b.height / 2.f);
    t.setPosition(window.getSize().x / 2.f, y);
    window.draw(t);
}

} // namespace

GameScreens::GameScreens() {
    fontLoaded = FontLoader::load(font);
    hoverHighlight.setFillColor(sf::Color(255, 230, 100, 70));
    hoverHighlight.setOutlineColor(sf::Color(255, 240, 160, 140));
    hoverHighlight.setOutlineThickness(2.f);
}

void GameScreens::buildPauseButtons(sf::Vector2u windowSize) {
    pauseButtons.clear();

    const float width = windowSize.x * 0.34f;
    const float height = windowSize.y * 0.07f;
    const float left = (windowSize.x - width) * 0.5f;
    float top = windowSize.y * 0.42f;
    const float step = windowSize.y * 0.11f;

    pauseButtons.push_back({{left, top, width, height}, "Resume"});
    top += step;
    pauseButtons.push_back({{left, top, width, height}, "Return to Main Menu"});
}

void GameScreens::buildVictoryButtons(sf::Vector2u windowSize) {
    victoryButtons.clear();

    const float width = windowSize.x * 0.42f;
    const float height = windowSize.y * 0.075f;
    const float left = (windowSize.x - width) * 0.5f;
    const float top = windowSize.y * 0.62f;

    victoryButtons.push_back(
        {{left, top, width, height}, "Volver al men\u00fa principal"}
    );
}

void GameScreens::preparePauseMenu(sf::Vector2u windowSize) {
    buildPauseButtons(windowSize);
    pauseHovered = -1;
}

void GameScreens::prepareVictoryScreen(
    sf::Vector2u windowSize,
    const std::string& playerName,
    int finalScore,
    int bestScore
) {
    victoryPlayerName = playerName;
    victoryFinalScore = finalScore;
    victoryBestScore = bestScore;
    buildVictoryButtons(windowSize);
    victoryHovered = -1;
}

void GameScreens::updatePauseMenu(sf::Vector2f mousePos) {
    pauseHovered = -1;
    for (size_t i = 0; i < pauseButtons.size(); ++i) {
        if (pauseButtons[i].bounds.contains(mousePos)) {
            pauseHovered = static_cast<int>(i);
            break;
        }
    }
}

void GameScreens::updateVictoryMenu(sf::Vector2f mousePos) {
    victoryHovered = -1;
    for (size_t i = 0; i < victoryButtons.size(); ++i) {
        if (victoryButtons[i].bounds.contains(mousePos)) {
            victoryHovered = static_cast<int>(i);
            break;
        }
    }
}

PauseMenuAction GameScreens::handlePauseEvent(
    const sf::Event& event,
    sf::RenderWindow& window
) {
    if (event.type == sf::Event::MouseMoved) {
        updatePauseMenu(sf::Vector2f(
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
        updatePauseMenu(mouse);

        if (pauseHovered == 0) return PauseMenuAction::Resume;
        if (pauseHovered == 1) return PauseMenuAction::MainMenu;
    }

    if (event.type == sf::Event::KeyPressed &&
        event.key.code == sf::Keyboard::Escape) {
        return PauseMenuAction::Resume;
    }

    (void)window;
    return PauseMenuAction::None;
}

VictoryMenuAction GameScreens::handleVictoryEvent(
    const sf::Event& event,
    sf::RenderWindow& window
) {
    if (event.type == sf::Event::MouseMoved) {
        updateVictoryMenu(sf::Vector2f(
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
        updateVictoryMenu(mouse);

        if (victoryHovered == 0) return VictoryMenuAction::MainMenu;
    }

    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Enter ||
            event.key.code == sf::Keyboard::Escape) {
            return VictoryMenuAction::MainMenu;
        }
    }

    (void)window;
    return VictoryMenuAction::None;
}

void GameScreens::drawOverlay(sf::RenderWindow& window, sf::Color color) const {
    sf::RectangleShape overlay;
    overlay.setSize(sf::Vector2f(window.getSize()));
    overlay.setFillColor(color);
    window.draw(overlay);
}

void GameScreens::drawMenuPanel(
    sf::RenderWindow& window,
    const std::string& title,
    const std::vector<MenuButton>& buttons,
    int hoveredIndex
) const {
    if (!fontLoaded) return;

    drawOverlay(window, sf::Color(0, 0, 0, 170));
    drawCenteredText(window, font, title, 220.f, 48, sf::Color::White);

    for (size_t i = 0; i < buttons.size(); ++i) {
        const auto& btn = buttons[i];
        if (static_cast<int>(i) == hoveredIndex) {
            sf::RectangleShape highlight;
            highlight.setPosition(btn.bounds.left, btn.bounds.top);
            highlight.setSize({btn.bounds.width, btn.bounds.height});
            highlight.setFillColor(sf::Color(255, 230, 100, 70));
            highlight.setOutlineColor(sf::Color(255, 240, 160, 140));
            highlight.setOutlineThickness(2.f);
            window.draw(highlight);
        }
        drawCenteredText(
            window,
            font,
            btn.label,
            btn.bounds.top + btn.bounds.height * 0.5f,
            26,
            sf::Color(230, 230, 230)
        );
    }
}

void GameScreens::drawPause(sf::RenderWindow& window) {
    drawMenuPanel(window, "PAUSED", pauseButtons, pauseHovered);
}

void GameScreens::drawGameOver(sf::RenderWindow& window) {
    if (!fontLoaded) return;
    drawOverlay(window, sf::Color(40, 0, 0, 180));
    drawCenteredText(window, font, "GAME OVER", 300.f, 52, sf::Color::Red);
    drawCenteredText(window, font, "Press R to retry", 370.f, 22, sf::Color::White);
}

void GameScreens::drawVictory(sf::RenderWindow& window) {
    if (!fontLoaded) return;

    drawOverlay(window, sf::Color(0, 20, 0, 170));
    drawCenteredText(window, font, "VICTORIA", 180.f, 64, sf::Color(120, 255, 120));
    drawCenteredText(
        window,
        font,
        "Naruto ha sido derrotado",
        250.f,
        28,
        sf::Color(220, 255, 220)
    );
    drawCenteredText(
        window,
        font,
        victoryPlayerName,
        310.f,
        24,
        sf::Color::White
    );
    drawCenteredText(
        window,
        font,
        "Puntuacion final: " + std::to_string(victoryFinalScore),
        350.f,
        24,
        sf::Color(255, 230, 120)
    );
    drawCenteredText(
        window,
        font,
        "Mejor puntuacion: " + std::to_string(victoryBestScore),
        385.f,
        22,
        sf::Color(200, 200, 200)
    );

    if (!victoryButtons.empty()) {
        const auto& btn = victoryButtons.front();
        if (victoryHovered == 0) {
            sf::RectangleShape highlight;
            highlight.setPosition(btn.bounds.left, btn.bounds.top);
            highlight.setSize({btn.bounds.width, btn.bounds.height});
            highlight.setFillColor(sf::Color(255, 230, 100, 90));
            highlight.setOutlineColor(sf::Color(255, 240, 160, 180));
            highlight.setOutlineThickness(2.f);
            window.draw(highlight);
        }
        drawCenteredText(
            window,
            font,
            btn.label,
            btn.bounds.top + btn.bounds.height * 0.5f,
            28,
            victoryHovered == 0 ? sf::Color::White : sf::Color(230, 230, 230)
        );
    }

    drawCenteredText(
        window,
        font,
        "ENTER o ESC",
        window.getSize().y * 0.72f,
        18,
        sf::Color(180, 180, 180)
    );
}
