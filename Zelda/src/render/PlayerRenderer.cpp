#include "PlayerRenderer.h"

#include <iostream>

// ========================================
// CONSTRUCTOR
// ========================================

PlayerRenderer::PlayerRenderer() {

    animationFrame = 1;

    animationTimer = 0.0f;

    // ========================================
    // INITIAL SPRITE
    // ========================================

    if (
        !texture.loadFromFile(
            "assets/sprites/player/move/walk_d1.png"
        )
    ) {

        std::cout
            << "SPRITE NOT FOUND"
            << std::endl;
    }

    sprite.setTexture(texture);

    // ========================================
    // SCALE
    // ========================================

    sprite.setScale(2.0f, 2.0f);
}

// ========================================
// UPDATE
// ========================================

void PlayerRenderer::update(
    Player& player,
    float deltaTime
) {

    animationTimer += deltaTime;

    // ========================================
    // ANIMATION TIMER
    // ========================================

    if (animationTimer >= 0.15f) {

        animationTimer = 0.0f;

        animationFrame++;

        if (animationFrame > 5)
            animationFrame = 1;
    }

    // ========================================
    // DIRECTION
    // ========================================

    std::string direction;

    switch (player.getDirection()) {

        case UP:

            direction = "u";

            break;

        case DOWN:

            direction = "d";

            break;

        case LEFT:

            direction = "l";

            break;

        case RIGHT:

            direction = "r";

            break;
    }

    // ========================================
    // PATH
    // ========================================

    std::string path =
        "assets/sprites/player/move/walk_"
        + direction
        + std::to_string(animationFrame)
        + ".png";

    // ========================================
    // LOAD TEXTURE
    // ========================================

    if (!texture.loadFromFile(path)) {

        std::cout
            << "FAILED TO LOAD: "
            << path
            << std::endl;
    }

    sprite.setTexture(texture);

    // ========================================
    // TILE SIZE
    // ========================================

    const int TILE_SIZE = 48;

    // ========================================
    // POSITION
    // ========================================

    sprite.setPosition(
        player.getX() * TILE_SIZE,
        player.getY() * TILE_SIZE
    );
}

// ========================================
// DRAW
// ========================================

void PlayerRenderer::draw(
    sf::RenderWindow& window
) {

    window.draw(sprite);
}