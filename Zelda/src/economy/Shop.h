#pragma once

#include "ShopItem.h"

#include <vector>

class Player;

// Randomized shop inventory per run visit.
class Shop {
public:
    void randomize(int seed);
    bool buy(Player& player, int index);
    const std::vector<ShopItem>& getItems() const { return items; }

private:
    std::vector<ShopItem> items;
};
