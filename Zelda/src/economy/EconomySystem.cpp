#include "EconomySystem.h"

void EconomySystem::openShop(Player& player, int seed) {
    (void)player;
    shop.randomize(seed);
}
