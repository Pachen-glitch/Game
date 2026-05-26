#include "Door.h"

#include "../player/Player.h"

#include "../../utils/AssetPaths.h"
#include "../../interaction/EventBus.h"

Door::Door(sf::Vector2f pos, bool isLocked)

    : Entity(
        pos,
        {48.f, 64.f},
        AssetPaths::getDoorSprite(),
        EntityType::Door
    ),
    locked(isLocked)
{
    sprite.setScale(2.f, 2.f);
}

void Door::unlock() {

    locked = false;

    sprite.setColor(sf::Color(120, 255, 120));

    EventBus::instance().emit("door_unlock");
}

bool Door::isLocked() const {

    return locked;
}

void Door::onInteract(Player& player) {

    if (!locked)
        return;

    if (player.useKey()) {

        unlock();
    }
}

void Door::update(float dt) {

    if (locked) {

        static float timer = 0.f;

        timer += dt;

        float glow = 200 + std::sin(timer * 4.f) * 55;

        sprite.setColor(
            sf::Color(glow, glow * 0.6f, glow * 0.6f)
        );
    }
}
