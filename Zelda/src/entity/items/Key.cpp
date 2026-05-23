#include "Key.h"
#include "../player/Player.h"
#include "../../utils/AssetPaths.h"

Key::Key(sf::Vector2f pos)
    : Entity(pos, {32.f, 32.f}, AssetPaths::getKeySprite(), EntityType::Key)
{}

void Key::onInteract(Player& player) {
    player.addKey();
    deactivate();
}

void Key::update(float) {}
