#pragma once

#include "../entity/base/Entity.h"

class Door : public Entity {

private:

    bool locked;

public:

    Door(int startX, int startY);

    void unlock();

    bool isLocked();

    void onInteract(Player& player) override;

    void update() override;
};