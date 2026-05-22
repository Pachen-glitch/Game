#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <vector>

class Room;

class Minimap {
public:
    void draw(
        sf::RenderWindow& window,
        const std::vector<Room>& rooms,
        int currentId
    );
};
