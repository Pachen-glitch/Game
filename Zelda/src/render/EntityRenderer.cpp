#include "EntityRenderer.h"

#include "../entity/player/Player.h"

void EntityRenderer::update(
    EntityManager& entities,
    float dt
) {

    for (auto it = slimeAnimators.begin();
         it != slimeAnimators.end();) {

        if (!it->first ||
            !it->first->isActive()) {

            it = slimeAnimators.erase(it);
        }
        else {

            ++it;
        }
    }

    for (auto it = skeletonAnimators.begin();
         it != skeletonAnimators.end();) {

        if (!it->first ||
            !it->first->isActive()) {

            it = skeletonAnimators.erase(it);
        }
        else {

            ++it;
        }
    }

    for (auto& ent : entities.all()) {

        if (!ent ||
            !ent->isActive())
            continue;

        // =====================
        // SLIME
        // =====================

        auto* slime =
            dynamic_cast<SlimeEnemy*>(
                ent.get()
            );

        if (slime) {

            auto [iter, inserted] =
                slimeAnimators.try_emplace(
                    slime
                );

            if (inserted) {

                iter->second.setupSlime();
            }

            iter->second.update(
                *slime,
                dt
            );

            iter->second.applyToEntity(
                *slime,
                2.f
            );

            continue;
        }

        // =====================
        // SKELETON
        // =====================

        auto* skeleton =
            dynamic_cast<SkeletonEnemy*>(
                ent.get()
            );

        if (skeleton) {

            auto [iter, inserted] =
                skeletonAnimators.try_emplace(
                    skeleton
                );

            iter->second.update(
                *skeleton,
                dt
            );

            continue;
        }
    }
}

void EntityRenderer::draw(
    sf::RenderWindow& window,
    const Player& player,
    EntityManager& entities
) {

    window.draw(
        player.getSprite()
    );

    for (auto& ent : entities.all()) {

        if (!ent ||
            !ent->isActive())
            continue;

        window.draw(
            ent->getSprite()
        );
    }
}
