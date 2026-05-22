#pragma once

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include "../economy/Shop.h"

class Player;

class ShopUI {
public:
    ShopUI();
    void draw(sf::RenderWindow& window, const Shop& shop, const Player& player);
    void handleInput(Shop& shop, Player& player);

private:
    sf::Font font;
    bool fontLoaded = false;
};
