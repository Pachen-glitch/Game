#include "Heart.h"
#include "../player/Player.h"

Heart::Heart(int startX, int startY)
    : Entity(
        startX,
        startY,
        64,
        64,
        "assets/sprites/objects/heart.png"
    )
{

}

void Heart::onInteract(Player& player) {

    player.heal(1);

    deactivate();
}

void Heart::update() {

}