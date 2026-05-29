#include "Chest.h"

#include "../player/Player.h"
#include "../base/EntityManager.h"

#include "Coin.h"

#include "../../utils/AssetPaths.h"
#include "../../render/TextureCache.h"
#include "../../interaction/EventBus.h"
#include <iostream>
#include <cmath>

Chest::Chest(
    sf::Vector2f pos,
    EntityManager* entityManager
)

    : Entity(

        pos,

        {220.f, 202.f},

        AssetPaths::getChestClosedSprite(),

        EntityType::Chest
    ),

    entities(entityManager)
{

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

    player.addRupees(25);

    EventBus::instance().emit("rupee_pickup");
}

void Chest::update(float dt) {

    if (!opened) {

        static float timer = 0.f;

        timer += dt;

        float offset =
            std::sin(timer * 2.5f) * 2.f;

        sprite.setPosition(
            position.x,
            position.y + offset
        );
    }
}