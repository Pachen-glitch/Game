#pragma once

#include "../base/Entity.h"

class Enemy : public Entity {

private:

    int health;

public:

    Enemy(int startX, int startY);

    void damage(int amount);

    bool isDead();

    void onInteract(Player& player) override;

    void update() override;
};