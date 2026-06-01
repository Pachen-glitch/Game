#include "Coin.h"

#include "../player/Player.h"

#include "../../utils/AssetPaths.h"
#include "../../interaction/EventBus.h"

#include <cmath>

Coin::Coin(sf::Vector2f pos)

    : Entity(
        pos,
        {24.f, 24.f},
        AssetPaths::getRandomRupeeSprite(),
        EntityType::Coin
    )
{
    sprite.setScale(1.6f, 1.6f);

    std::string texturePath = spritePath;

    if (texturePath.find("gema_azul") != std::string::npos) {

        value = 1;

        sprite.setScale(1.3f, 1.3f);

        sprite.setColor(
            sf::Color(120, 180, 255)
        );
    }
    else if (texturePath.find("gema_roja") != std::string::npos) {

        value = 5;

        sprite.setScale(1.7f, 1.7f);

        sprite.setColor(
            sf::Color(255, 120, 120)
        );
    }
    else if (texturePath.find("gema_diamond") != std::string::npos) {

        value = 20;

        sprite.setScale(2.3f, 2.3f);

        sprite.setColor(
            sf::Color(255, 255, 180)
        );
    }
    else {

        value = 1;

        sprite.setScale(1.4f, 1.4f);
    }
}

void Coin::onInteract(Player& player) {

    player.addRupees(value);

    EventBus::instance().emit("rupee_pickup");

    deactivate();
}

void Coin::update(float dt) {

    static float timer = 0.f;

    timer += dt;

    float speed = 4.f;

    float height = 2.f;

    if (value == 20) {

        speed = 2.f;

        height = 5.f;
    }

    float offset = std::sin(timer * speed) * height;

    sprite.setPosition(
        position.x,
        position.y + offset
    );
}

int Coin::getValue() const {

    return value;
}