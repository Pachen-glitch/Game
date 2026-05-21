#include "Coin.h"
#include "../entity/Player.h"

Coin::Coin(int startX, int startY)
    : Entity(
        startX,
        startY,
        64,
        64,
        "assets/sprites/objects/coin.png"
    )
{

}

void Coin::onInteract(Player& player) {

    player.addCoin();

    deactivate();
}

void Coin::update() {

}