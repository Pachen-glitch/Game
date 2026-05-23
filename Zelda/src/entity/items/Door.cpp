#include "Door.h"
#include "../player/Player.h"
#include "../../utils/AssetPaths.h"

Door::Door(sf::Vector2f pos, bool isLocked)
    : Entity(pos, {48.f, 48.f}, AssetPaths::getDoorSprite(), EntityType::Door)
    , locked(isLocked)
{}

void Door::unlock() { locked = false; }
bool Door::isLocked() const { return locked; }

void Door::onInteract(Player& player) {
    if (!locked) return;
    if (player.useKey()) unlock();
}

void Door::update(float) {}
