#pragma once

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

class GameScreens {
public:
    GameScreens();
    void drawPause(sf::RenderWindow& window);
    void drawGameOver(sf::RenderWindow& window);
    void drawVictory(sf::RenderWindow& window);

private:
    sf::Font font;
    bool fontLoaded = false;
};
