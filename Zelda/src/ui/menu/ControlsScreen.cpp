#include "ControlsScreen.h"

#include "../../utils/AssetPaths.h"

ControlsScreen::ControlsScreen() {
    setBackgroundPath(AssetPaths::getMenuFondoBackground());
}

void ControlsScreen::prepare(sf::Vector2u windowSize) {
    layoutForWindow(windowSize);
}

GameState ControlsScreen::handleEvent(
    const sf::Event& event,
    sf::RenderWindow& window
) {
    if (event.type == sf::Event::MouseMoved) {
        update(sf::Vector2f(
            static_cast<float>(event.mouseMove.x),
            static_cast<float>(event.mouseMove.y)
        ));
    }

    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mouse(
            static_cast<float>(event.mouseButton.x),
            static_cast<float>(event.mouseButton.y)
        );
        if (handleBackButton(window, mouse, true)) {
            return GameState::MainMenu;
        }
    }

    return GameState::Controls;
}

void ControlsScreen::update(sf::Vector2f mousePos) {
    backHovered = backButtonBounds.contains(mousePos);
}

void ControlsScreen::draw(sf::RenderWindow& window) const {
    drawBackground(window);
    drawTitle(window, "Controles", 80.f);
    drawLines(window, {
        "WASD  ->  Movimiento",
        "F     ->  Ataque",
        "ESC   ->  Pausa"
    }, 170.f, 28);
    drawBackButton(window, backHovered);
}
