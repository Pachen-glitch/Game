#pragma once

#include "MenuScreenBase.h"

#include "../../core/GameState.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

#include <string>

// Modal name entry before starting a new run.
class NameEntryScreen : public MenuScreenBase {
public:
    NameEntryScreen();

    void prepare(sf::Vector2u windowSize);
    GameState handleEvent(const sf::Event& event, sf::RenderWindow& window);
    void update(sf::Vector2f mousePos);
    void draw(sf::RenderWindow& window) const;

    const std::string& getEnteredName() const { return enteredName; }
    void resetInput();

private:
    void confirm();

    std::string inputText;
    std::string enteredName;
    sf::FloatRect panelBounds;
    sf::FloatRect inputBounds;
    sf::FloatRect confirmBounds;
    bool confirmHovered = false;
};
