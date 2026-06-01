#pragma once

#include "../entity/base/EntityManager.h"
#include "../entity/player/Player.h"

// Proximity pickup / NPC talk — uses onInteract on entities.
class InteractionSystem {
public:
    void update(float dt, Player& player, EntityManager& entities);
    void tryInteract(Player& player, EntityManager& entities);

    void startShopCooldown(float seconds);
    bool isShopOnCooldown() const;

private:
    float shopCooldown = 0.f;
};
