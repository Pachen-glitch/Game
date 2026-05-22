#pragma once

#include "../base/Entity.h"

class Heart : public Entity {
public:
    Heart(sf::Vector2f pos);
    void onInteract(Player& player) override;
    void update(float deltaTime) override;
};
