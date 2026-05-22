#pragma once

#include "../base/Entity.h"

class Shopkeeper : public Entity {
public:
    Shopkeeper(sf::Vector2f pos);
    void onInteract(Player& player) override;
    void update(float deltaTime) override;
};
