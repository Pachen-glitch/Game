#include "Door.h"
#include "../entity/Player.h"

Door::Door(int startX, int startY)
    : Entity(
        startX,
        startY,
        64,
        64,
        "assets/sprites/objects/door.png"
    )
{
    locked = true;
}

void Door::unlock() {

    locked = false;
}

bool Door::isLocked() {

    return locked;
}

void Door::onInteract(Player& player) {

    if (player.getKeys() > 0) {

        unlock();
    }
}

void Door::update() {

}