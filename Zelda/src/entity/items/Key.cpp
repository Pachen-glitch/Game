#include "Key.h"
#include "../player/Player.h"

Key::Key(sf::Vector2f pos)
    : Entity(pos, {32.f, 32.f}, "assets/sprites/objects/key.png", EntityType::Key)
{}

void Key::onInteract(Player& player) {
    player.addKey();
    deactivate();
}

void Key::update(float) {}
