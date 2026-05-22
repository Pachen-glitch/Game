#pragma once

#include <SFML/Graphics/RenderWindow.hpp>

#include "../entity/base/EntityManager.h"
#include "../entity/player/Player.h"

class EntityRenderer {
public:
    void draw(sf::RenderWindow& window, const Player& player, EntityManager& entities);
};
