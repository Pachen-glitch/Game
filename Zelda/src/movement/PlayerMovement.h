#pragma once

#include "../entity/Player.h"
#include "../map/Map.h"

class PlayerMovement {

public:

    static void move(
        Player& player,
        char input,
        Map& map
    );
};