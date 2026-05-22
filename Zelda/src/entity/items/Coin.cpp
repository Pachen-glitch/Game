#include "Coin.h"
#include "../player/Player.h"

Coin::Coin(sf::Vector2f pos)
    : Entity(pos, {32.f, 32.f}, "assets/sprites/objects/coin.png", EntityType::Coin)
{}

void Coin::onInteract(Player& player) {
    player.addRupees(1);
    deactivate();
}

void Coin::update(float) {}
