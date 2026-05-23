#include "Heart.h"
#include "../player/Player.h"
#include "../../utils/AssetPaths.h"

Heart::Heart(sf::Vector2f pos)
    : Entity(pos, {32.f, 32.f}, AssetPaths::getHeartSprite(), EntityType::Heart)
{}

void Heart::onInteract(Player& player) {
    player.heal(1);
    deactivate();
}

void Heart::update(float) {}
