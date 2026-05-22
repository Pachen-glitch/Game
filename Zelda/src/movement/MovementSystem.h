#pragma once

#include "../entity/player/Player.h"
#include "../map/Map.h"

// Smooth WASD movement + action input routing.
class MovementSystem {
public:
    void update(float dt, Player& player, const Map& map);

private:
    bool attackPressed = false;
    bool spinPressed = false;
};
