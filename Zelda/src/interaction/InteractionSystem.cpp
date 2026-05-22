#include "InteractionSystem.h"
#include "../utils/MathUtils.h"

void InteractionSystem::update(Player& player, EntityManager& entities) {
    sf::Vector2f center = player.getPosition() + player.getSize() * 0.5f;

    for (auto& ent : entities.all()) {
        if (!ent || !ent->isActive()) continue;
        if (ent->getType() == EntityType::Enemy) continue;

        sf::Vector2f ec = ent->getPosition() + ent->getSize() * 0.5f;
        if (MathUtils::distance(center, ec) < 36.f) {
            ent->onInteract(player);
        }
    }
    entities.removeInactive();
}

void InteractionSystem::tryInteract(Player& player, EntityManager& entities) {
    sf::Vector2f center = player.getPosition() + player.getSize() * 0.5f;
    float best = 9999.f;
    Entity* target = nullptr;

    for (auto& ent : entities.all()) {
        if (!ent || !ent->isActive()) continue;
        sf::Vector2f ec = ent->getPosition() + ent->getSize() * 0.5f;
        float d = MathUtils::distance(center, ec);
        if (d < 64.f && d < best) {
            best = d;
            target = ent.get();
        }
    }

    if (target) target->onInteract(player);
    entities.removeInactive();
}
