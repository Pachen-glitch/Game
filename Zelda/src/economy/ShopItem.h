#pragma once

#include <functional>
#include <string>

class Player;

struct ShopItem {
    std::string name;
    int cost = 0;
    std::function<void(Player&)> apply;
};
