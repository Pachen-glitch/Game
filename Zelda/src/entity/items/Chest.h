#pragma once

#include "../base/Entity.h"

class EntityManager;

class Chest : public Entity {

private:

    bool opened = false;
    bool variedLoot = false;

    EntityManager* entities;

public:

    Chest(
        sf::Vector2f pos,
        EntityManager* entityManager,
        bool randomReward = false
    );

    void onInteract(Player& player) override;

    void update(float dt) override;
};