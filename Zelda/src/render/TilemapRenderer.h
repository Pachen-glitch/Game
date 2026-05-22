#pragma once

#include <SFML/Graphics/RenderWindow.hpp>

class Map;

class TilemapRenderer {
public:
    void draw(sf::RenderWindow& window, const Map& map);
};
