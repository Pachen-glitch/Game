#include "EntityRenderer.h"

void EntityRenderer::update(EntityManager& entities, float dt) {
    for (auto it = slimeAnimators.begin(); it != slimeAnimators.end();) {
        if (!it->first || !it->first->isActive()) {
            it = slimeAnimators.erase(it);
        } else {
            ++it;
        }
    }

    for (auto& ent : entities.all()) {
        if (!ent || !ent->isActive()) continue;

        auto* slime = dynamic_cast<SlimeEnemy*>(ent.get());
        if (!slime) continue;

        auto [iter, inserted] = slimeAnimators.try_emplace(slime);
        if (inserted) {
            iter->second.setupSlime();
        }

        iter->second.update(*slime, dt);
        iter->second.applyToEntity(*slime, 2.f);
    }
}

void EntityRenderer::draw(
    sf::RenderWindow& window,
    const Player& player,
    EntityManager& entities
) {
    window.draw(player.getSprite());

    for (auto& ent : entities.all()) {
        if (!ent || !ent->isActive()) continue;
        window.draw(ent->getSprite());
    }
}
