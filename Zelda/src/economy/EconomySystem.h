#pragma once

#include "Shop.h"
#include "../entity/player/Player.h"

class EconomySystem {
public:
    Shop& getShop() { return shop; }
    void openShop(Player& player, int seed);

private:
    Shop shop;
};
