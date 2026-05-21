#include "Game.h"

#include <SFML/Graphics.hpp>

#include "../map/Map.h"

#include "../entity/player/Player.h"

#include "../movement/PlayerMovement.h"

using namespace sf;

// ========================================
// GAME LOOP
// ========================================

void runGame() {

    // ========================================
    // WINDOW
    // ========================================

    RenderWindow window(
        VideoMode(1280, 720),
        "The Legend of Pachen"
    );

    window.setFramerateLimit(60);

    // ========================================
    // MAP
    // ========================================

    Map map;

    // ========================================
    // PLAYER
    // ========================================

    Player player(5, 5);

    // ========================================
    // GAME LOOP
    // ========================================

    while (window.isOpen()) {

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
        // ATTACKS
        // ========================================

        if (Keyboard::isKeyPressed(Keyboard::C)) {

            player.swordAttack();
        }

        if (Keyboard::isKeyPressed(Keyboard::X)) {

            player.spinAttack();
        }

        if (Keyboard::isKeyPressed(Keyboard::V)) {

            player.activateShield();
        }

        // ========================================
        // UPDATE
        // ========================================

        player.update();

        // ========================================
        // RENDER
        // ========================================

        window.clear();

        // luego:
        // renderMap()
        // renderEntities()
        // renderUI()

        window.display();
    }
}