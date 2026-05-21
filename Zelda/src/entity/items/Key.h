#pragma once

#include "../base/Entity.h"

class Key : public Entity {

public:

    Key(int startX, int startY);

    void onInteract(Player& player) override;

    void update() override;
};