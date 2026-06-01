#pragma once

#include <SFML/Graphics/View.hpp>
#include <SFML/System/Vector2.hpp>

class Player;
class Map;

// Smooth follow camera clamped to the active room.
class Camera {
public:
    void update(float dt, const Player& player, const Map& map, sf::Vector2u windowSize);
    const sf::View& getView() const { return view; }

private:
    sf::View view;
    sf::Vector2f position;
};
