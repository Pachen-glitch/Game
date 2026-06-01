#pragma once

#include "MenuScreenBase.h"

#include "../../core/GameState.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

class ControlsScreen : public MenuScreenBase {
public:
    ControlsScreen();

    void prepare(sf::Vector2u windowSize);
    GameState handleEvent(const sf::Event& event, sf::RenderWindow& window);
    void update(sf::Vector2f mousePos);
    void draw(sf::RenderWindow& window) const;

private:
    bool backHovered = false;
};
