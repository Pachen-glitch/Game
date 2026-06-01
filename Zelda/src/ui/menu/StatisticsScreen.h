#pragma once

#include "MenuScreenBase.h"

#include "../../core/GameState.h"
#include "../../save/SaveSystem.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

class StatisticsScreen : public MenuScreenBase {
public:
    StatisticsScreen();

    void prepare(sf::Vector2u windowSize, const SaveData& data);
    GameState handleEvent(const sf::Event& event, sf::RenderWindow& window);
    void update(sf::Vector2f mousePos);
    void draw(sf::RenderWindow& window) const;

private:
    bool backHovered = false;
    std::vector<std::string> statLines;
};
