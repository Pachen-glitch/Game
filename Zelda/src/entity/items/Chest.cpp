#include "Chest.h"

#include "../player/Player.h"
#include "../base/EntityManager.h"

#include "Coin.h"

#include "../../utils/AssetPaths.h"
#include "../../render/TextureCache.h"
#include "../../interaction/EventBus.h"
#include <cstdlib>
#include <iostream>
#include <cmath>

Chest::Chest(
    sf::Vector2f pos,
    EntityManager* entityManager,
    bool randomReward
)

    : Entity(

        pos,

        {220.f, 202.f},

        AssetPaths::getChestClosedSprite(),

        EntityType::Chest
    ),

    entities(entityManager),
    variedLoot(randomReward)
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

    if (variedLoot) {
        switch (std::rand() % 4) {
            case 0:
                player.addRupees(25);
                EventBus::instance().emit("rupee_pickup");
                break;
            case 1:
                player.heal(1);
                break;
            case 2:
                player.addKey();
                break;
            default:
                player.addRupees(10);
                EventBus::instance().emit("rupee_pickup");
                break;
        }
        return;
    }

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