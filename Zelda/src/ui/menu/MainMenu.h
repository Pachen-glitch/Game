#pragma once

#include "../../core/GameState.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Window/Event.hpp>

#include <vector>

// Interactive title screen over assets/menu/start.png.
class MainMenu {
public:
    MainMenu();

    void layoutForWindow(sf::Vector2u windowSize);

    // Returns next GameState when an option is chosen; nullopt if unchanged.
    struct MenuResult {
        GameState nextState = GameState::MainMenu;
        bool closeApp = false;
    };

    MenuResult handleEvent(const sf::Event& event, sf::RenderWindow& window);
    void update(sf::Vector2f mousePos);
    void draw(sf::RenderWindow& window) const;

private:
    struct HitButton {
        sf::FloatRect bounds;
        GameState targetState = GameState::MainMenu;
        bool exitApp = false;
    };

    void buildHitAreas(sf::Vector2u windowSize);

    sf::Sprite background;
    sf::RectangleShape hoverHighlight;
    std::vector<HitButton> buttons;
    int hoveredIndex = -1;
    int lastHoveredIndex = -1;
};
