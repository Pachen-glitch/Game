#include "Door.h"
#include "../player/Player.h"

Door::Door(sf::Vector2f pos, bool isLocked)
    : Entity(pos, {48.f, 48.f}, "assets/sprites/objects/door.png", EntityType::Door)
    , locked(isLocked)
{}

void Door::unlock() { locked = false; }

bool Door::isLocked() const { return locked; }

void Door::onInteract(Player& player) {
    if (!locked) return;
    if (player.useKey()) {
        unlock();
    }
}

void Door::update(float) {}
