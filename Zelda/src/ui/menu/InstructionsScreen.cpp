#include "InstructionsScreen.h"

#include "../../utils/AssetPaths.h"

InstructionsScreen::InstructionsScreen() {
    setBackgroundPath(AssetPaths::getMenuFondoBackground());
}

void InstructionsScreen::prepare(sf::Vector2u windowSize) {
    layoutForWindow(windowSize);
}

GameState InstructionsScreen::handleEvent(
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

    return GameState::Instructions;
}

void InstructionsScreen::update(sf::Vector2f mousePos) {
    backHovered = backButtonBounds.contains(mousePos);
}

void InstructionsScreen::draw(sf::RenderWindow& window) const {
    drawBackground(window);
    drawTitle(window, "Instrucciones", 80.f);
    drawLines(window, {
        "- Explora salas proceduralmente.",
        "- Derrota enemigos.",
        "- Consigue rupias.",
        "- Encuentra llaves.",
        "- Abre puertas.",
        "- Compra mejoras.",
        "- Sobrevive hasta el jefe final."
    }, 160.f, 26);
    drawBackButton(window, backHovered);
}
