#pragma once

#include "../base/Entity.h"

class Door : public Entity {
public:
    Door(sf::Vector2f pos, bool locked = false);

    void unlock();
    bool isLocked() const;
    int getTargetRoom() const { return targetRoom; }
    void setTargetRoom(int id) { targetRoom = id; }

    void onInteract(Player& player) override;
    void update(float deltaTime) override;

private:
    bool locked = false;
    int targetRoom = -1;
};
