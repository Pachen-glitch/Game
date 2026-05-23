#pragma once

#include <SFML/System/Vector2.hpp>

class Map;
class Enemy;

// Tile collision + room bounds for enemy movement.
class EnemyMovement {
public:
    static sf::Vector2f moveWithCollision(
        const Map& map,
        sf::Vector2f from,
        sf::Vector2f velocity,
        float dt,
        sf::Vector2f size
    );

    static sf::Vector2f clampToRoom(
        sf::Vector2f pos,
        sf::Vector2f size,
        const Map& map
    );
};
