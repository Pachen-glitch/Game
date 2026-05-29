#pragma once

#include "../base/Entity.h"

class Coin : public Entity {

private:

    int value = 1;

public:

    Coin(sf::Vector2f pos);

    void onInteract(Player& player) override;

    void update(float dt) override;

    int getValue() const;
};