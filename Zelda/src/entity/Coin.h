#pragma once

#include "../entity/Entity.h"

class Coin : public Entity {

public:

    Coin(int startX, int startY);

    void onInteract(Player& player) override;

    void update() override;
};