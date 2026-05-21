#include "Game.h"

#include <SFML/Graphics.hpp>

#include <filesystem>

#include "../map/Map.h"

#include "../entity/player/Player.h"

#include "../movement/PlayerMovement.h"

#include "../render/PlayerRenderer.h"

using namespace sf;

// ========================================
// GAME LOOP
// ========================================

void runGame() {

    // ========================================
    // WORKING DIRECTORY
    // ========================================
    std::filesystem::current_path("Debug");
    //std::filesystem::current_path("../../");

    // ========================================
    // WINDOW
    // ========================================

    RenderWindow window(
        VideoMode(1280, 720),
        "The Legend of Pachen"
    );

    window.setFramerateLimit(60);

    // ========================================
    // CLOCK
    // ========================================

    Clock clock;

    // ========================================
    // MAP
    // ========================================

    Map map;

    // ========================================
    // PLAYER
    // ========================================

    Player player(5, 5);

    PlayerRenderer playerRenderer;

    // ========================================
    // LOOP
    // ========================================

    while (window.isOpen()) {

        float deltaTime =
            clock.restart().asSeconds();

        // ========================================
        // EVENTS
        // ========================================

        Event event;

        while (window.pollEvent(event)) {

            if (
                event.type == Event::Closed
            ) {
                window.close();
            }
        }

        // ========================================
        // INPUT
        // ========================================

        if (Keyboard::isKeyPressed(Keyboard::W)) {

            PlayerMovement::move(
                player,
                'w',
                map
            );
        }

        if (Keyboard::isKeyPressed(Keyboard::S)) {

            PlayerMovement::move(
                player,
                's',
                map
            );
        }

        if (Keyboard::isKeyPressed(Keyboard::A)) {

            PlayerMovement::move(
                player,
                'a',
                map
            );
        }

        if (Keyboard::isKeyPressed(Keyboard::D)) {

            PlayerMovement::move(
                player,
                'd',
                map
            );
        }

        // ========================================
        // UPDATE
        // ========================================

        playerRenderer.update(
            player,
            deltaTime
        );

        // ========================================
        // RENDER
        // ========================================

        window.clear();

        playerRenderer.draw(window);

        window.display();
    }
}