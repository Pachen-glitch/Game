#pragma once

#include <SFML/System/Vector2.hpp>

class Map;

// Tile collision for smooth movement (Zelda-style, not maze-snapped).
class CollisionSystem {
public:
    static bool isWall(const Map& map, sf::Vector2f worldPos);
    static sf::Vector2f resolveMovement(
        const Map& map,
        sf::Vector2f from,
        sf::Vector2f to,
        sf::Vector2f size
    );
};
