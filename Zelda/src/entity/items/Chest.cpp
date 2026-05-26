#include "Chest.h"

#include "../player/Player.h"
#include "../base/EntityManager.h"
#include "Coin.h"

#include "../../utils/AssetPaths.h"
#include "../../render/TextureCache.h"
#include "../../interaction/EventBus.h"

Chest::Chest(
    sf::Vector2f pos,
    EntityManager* entityManager
)

    : Entity(
        pos,
        {48.f, 48.f},
        AssetPaths::getChestClosedSprite(),
        EntityType::Chest
    ),
    entities(entityManager)
{
    sprite.setScale(2.f, 2.f);
}

void Chest::onInteract(Player& player) {

    if (opened)
        return;

    opened = true;

    sprite.setTexture(
        TextureCache::instance().get(
            AssetPaths::getChestOpenSprite()
        )
    );

    for (int i = 0; i < 5; ++i) {

        sf::Vector2f offset(

            (rand() % 80) - 40.f,
            (rand() % 80) - 40.f
        );

        entities->spawn<Coin>(
            position + offset
        );
    }

    EventBus::instance().emit("rupee_pickup");
}

void Chest::update(float dt) {

    if (!opened) {

        static float timer = 0.f;

        timer += dt;

        float offset = std::sin(timer * 2.5f) * 1.5f;

        sprite.setPosition(
            position.x,
            position.y + offset
        );
    }
}