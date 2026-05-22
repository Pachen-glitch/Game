#pragma once

#include "../entity/base/EntityManager.h"
#include "../entity/player/Player.h"

// Proximity pickup / NPC talk — uses onInteract on entities.
class InteractionSystem {
public:
    void update(Player& player, EntityManager& entities);
    void tryInteract(Player& player, EntityManager& entities);
};
