#pragma once

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

class Player;

class HUD {
public:
    HUD();
    void draw(sf::RenderWindow& window, const Player& player);

private:
    sf::Font font;
    bool fontLoaded = false;
};
