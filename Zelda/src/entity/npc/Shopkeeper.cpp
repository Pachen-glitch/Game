#include "Shopkeeper.h"
#include "../player/Player.h"
#include "../../interaction/EventBus.h"

Shopkeeper::Shopkeeper(sf::Vector2f pos)
    : Entity(pos, {48.f, 48.f}, "assets/sprites/npc/shopkeeper.png", EntityType::Shopkeeper)
{}

void Shopkeeper::onInteract(Player&) {
    EventBus::instance().emit("open_shop");
}

void Shopkeeper::update(float) {}
