#pragma once

#include "../base/Entity.h"

class Heart : public Entity {

public:

    Heart(int startX, int startY);

    void onInteract(Player& player) override;

    void update() override;
};