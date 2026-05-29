#include "Shopkeeper.h"
#include "../player/Player.h"
#include "../../interaction/EventBus.h"
#include "../../utils/AssetPaths.h"
#include <cmath>

Shopkeeper::Shopkeeper(sf::Vector2f pos)
    : Entity(
        pos,
        {102.f, 120.f},
        AssetPaths::getShopkeeperSprite(),
        EntityType::Shopkeeper
    )
{
    sprite.setScale(1.5f, 1.5f);
}

void Shopkeeper::onInteract(Player&) {
    EventBus::instance().emit("open_shop");
}

void Shopkeeper::update(float dt) {

    static float timer = 0.f;

    timer += dt;

    // Flotación
    float offset =
        std::sin(timer * 2.f) * 6.f;

    sprite.setPosition(
        position.x,
        position.y + offset
    );

    // Brillo pulsante
    float glow =
        220.f + std::sin(timer * 3.f) * 35.f;

    sprite.setColor(
        sf::Color(
            static_cast<sf::Uint8>(glow),
            static_cast<sf::Uint8>(glow),
            static_cast<sf::Uint8>(200)
        )
    );
}