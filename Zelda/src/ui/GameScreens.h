#pragma once

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>

#include <string>
#include <vector>

enum class PauseMenuAction {
    None,
    Resume,
    MainMenu
};

enum class VictoryMenuAction {
    None,
    MainMenu
};

class GameScreens {
public:
    GameScreens();

    void preparePauseMenu(sf::Vector2u windowSize);
    void prepareVictoryScreen(
        sf::Vector2u windowSize,
        const std::string& playerName,
        int finalScore,
        int bestScore
    );

    PauseMenuAction handlePauseEvent(
        const sf::Event& event,
        sf::RenderWindow& window
    );
    VictoryMenuAction handleVictoryEvent(
        const sf::Event& event,
        sf::RenderWindow& window
    );

    void updatePauseMenu(sf::Vector2f mousePos);
    void updateVictoryMenu(sf::Vector2f mousePos);

    void drawPause(sf::RenderWindow& window);
    void drawGameOver(sf::RenderWindow& window);
    void drawVictory(sf::RenderWindow& window);

private:
    struct MenuButton {
        sf::FloatRect bounds;
        std::string label;
    };

    void buildPauseButtons(sf::Vector2u windowSize);
    void buildVictoryButtons(sf::Vector2u windowSize);
    void drawOverlay(sf::RenderWindow& window, sf::Color color) const;
    void drawMenuPanel(
        sf::RenderWindow& window,
        const std::string& title,
        const std::vector<MenuButton>& buttons,
        int hoveredIndex
    ) const;

    sf::Font font;
    bool fontLoaded = false;

    std::vector<MenuButton> pauseButtons;
    std::vector<MenuButton> victoryButtons;
    int pauseHovered = -1;
    int victoryHovered = -1;

    std::string victoryPlayerName;
    int victoryFinalScore = 0;
    int victoryBestScore = 0;

    sf::RectangleShape hoverHighlight;
};
