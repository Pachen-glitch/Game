#include "Shopkeeper.h"
#include "../player/Player.h"
#include "../../interaction/EventBus.h"
#include "../../utils/AssetPaths.h"

Shopkeeper::Shopkeeper(sf::Vector2f pos)
    : Entity(pos, {48.f, 48.f}, AssetPaths::getShopkeeperSprite(), EntityType::Shopkeeper)
{}

void Shopkeeper::onInteract(Player&) {
    EventBus::instance().emit("open_shop");
}

void Shopkeeper::update(float) {}
