#include "GameScreens.h"
#include "../utils/FontLoader.h"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>

GameScreens::GameScreens() {
    fontLoaded = FontLoader::load(font);
}

static void drawCenteredText(
    sf::RenderWindow& window,
    sf::Font& font,
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

void GameScreens::drawPause(sf::RenderWindow& window) {
    if (!fontLoaded) return;
    sf::RectangleShape overlay;
    overlay.setSize(sf::Vector2f(window.getSize()));
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
    window.draw(overlay);
    drawCenteredText(window, font, "PAUSED", 300.f, 48, sf::Color::White);
    drawCenteredText(window, font, "ESC to resume", 360.f, 22, sf::Color(200, 200, 200));
}

void GameScreens::drawGameOver(sf::RenderWindow& window) {
    if (!fontLoaded) return;
    sf::RectangleShape overlay;
    overlay.setSize(sf::Vector2f(window.getSize()));
    overlay.setFillColor(sf::Color(40, 0, 0, 180));
    window.draw(overlay);
    drawCenteredText(window, font, "GAME OVER", 300.f, 52, sf::Color::Red);
    drawCenteredText(window, font, "Press R to retry", 370.f, 22, sf::Color::White);
}

void GameScreens::drawVictory(sf::RenderWindow& window) {
    if (!fontLoaded) return;
    sf::RectangleShape overlay;
    overlay.setSize(sf::Vector2f(window.getSize()));
    overlay.setFillColor(sf::Color(0, 40, 0, 160));
    window.draw(overlay);
    drawCenteredText(window, font, "VICTORY!", 300.f, 52, sf::Color::Green);
}
