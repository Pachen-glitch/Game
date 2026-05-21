#pragma once

#include <SFML/Graphics.hpp>

#include "../entity/player/Player.h"

class PlayerRenderer {

private:

    sf::Texture texture;

    sf::Sprite sprite;

    int animationFrame;

    float animationTimer;

public:

    PlayerRenderer();

    void update(
        Player& player,
        float deltaTime
    );

    void draw(
        sf::RenderWindow& window
    );
};