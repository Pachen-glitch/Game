#include "Coin.h"

#include "../player/Player.h"

#include "../../utils/AssetPaths.h"
#include "../../interaction/EventBus.h"

Coin::Coin(sf::Vector2f pos)

    : Entity(
        pos,
        {24.f, 24.f},
        AssetPaths::getRandomRupeeSprite(),
        EntityType::Coin
    )
{
    sprite.setScale(1.6f, 1.6f);
}

void Coin::onInteract(Player& player) {

    player.addRupees(5);

    EventBus::instance().emit("rupee_pickup");

    deactivate();
}

void Coin::update(float dt) {

    static float timer = 0.f;

    timer += dt;

    float offset = std::sin(timer * 4.f) * 2.f;

    sprite.setPosition(
        position.x,
        position.y + offset
    );
}