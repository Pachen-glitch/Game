#pragma once

#include "../entity/player/Player.h"
#include "../map/Map.h"

// Legacy tile step API — kept for compatibility; prefer MovementSystem.
class PlayerMovement {
public:
    static void move(Player& player, char input, Map& map);
};
