#include "EntityRenderer.h"

void EntityRenderer::draw(
    sf::RenderWindow& window,
    const Player& player,
    EntityManager& entities
) {
    window.draw(player.getSprite());

    for (auto& ent : entities.all()) {
        if (ent && ent->isActive()) {
            window.draw(ent->getSprite());
        }
    }
}
