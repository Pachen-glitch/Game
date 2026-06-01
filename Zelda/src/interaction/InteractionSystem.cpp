#include "InteractionSystem.h"

#include "../core/Constants.h"
#include "../entity/base/Entity.h"
#include "../utils/MathUtils.h"

#include <algorithm>
// Establece el cooldown de la tienda
void InteractionSystem::startShopCooldown(float seconds) {
    shopCooldown = seconds;
}
// Devuelve si la tienda esta en cooldown
bool InteractionSystem::isShopOnCooldown() const {
    return shopCooldown > 0.f;
}

void InteractionSystem::update(float dt, Player& player, EntityManager& entities) {
    shopCooldown = std::max(0.f, shopCooldown - dt);

    sf::Vector2f center = player.getPosition() + player.getSize() * 0.5f;

    for (auto& ent : entities.all()) {
        if (!ent || !ent->isActive()) continue;
        if (ent->getType() == EntityType::Enemy) continue;
        if (ent->getType() == EntityType::Shopkeeper && shopCooldown > 0.f) continue;

        sf::Vector2f ec = ent->getPosition() + ent->getSize() * 0.5f;
        if (MathUtils::distance(center, ec) < 36.f) {
            ent->onInteract(player);
        }
    }
    entities.removeInactive();
}
// Intenta interactuar con el jugador
void InteractionSystem::tryInteract(Player& player, EntityManager& entities) { // Intenta interactuar con el jugador
    if (shopCooldown > 0.f) return;

    sf::Vector2f center = player.getPosition() + player.getSize() * 0.5f;
    float best = 9999.f;
    Entity* target = nullptr;

    for (auto& ent : entities.all()) {
        if (!ent || !ent->isActive()) continue;
        if (ent->getType() == EntityType::Shopkeeper && shopCooldown > 0.f) continue;
// Si el entity es un shopkeeper y el cooldown de la tienda es mayor a 0, no se intenta interactuar
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
