#pragma once

#include "../base/Entity.h"

class Door : public Entity {

private:

    bool locked = true;

public:

    Door(
        sf::Vector2f pos,
        bool isLocked = true
    );

    void unlock();

    bool isLocked() const;

    bool canPass() const;

    void onInteract(Player& player) override;

    void update(float dt) override;
};