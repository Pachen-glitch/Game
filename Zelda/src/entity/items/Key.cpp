#include "Key.h"
#include "../player/Player.h"

Key::Key(int startX, int startY)
    : Entity(
        startX,
        startY,
        64,
        64,
        "assets/sprites/objects/key.png"
    )
{

}

void Key::onInteract(Player& player) {

    player.addKey();

    deactivate();
}

void Key::update() {

}