#include "EntityRenderer.h"

#include "../entity/player/Player.h"

namespace {

bool isEntityRegistered(const EntityManager& entities, Entity* ptr) {
    if (!ptr) return false;
    for (const auto& ent : entities.all()) {
        if (ent.get() == ptr) return true;
    }
    return false;
}

template<typename Map>
void pruneAnimatorMap(Map& animators, const EntityManager& entities) {
    for (auto it = animators.begin(); it != animators.end();) {
        if (!isEntityRegistered(entities, it->first)) {
            it = animators.erase(it);
            continue;
        }
        if (!it->first->isActive()) {
            it = animators.erase(it);
            continue;
        }
        ++it;
    }
}

} // namespace

void EntityRenderer::update(
    EntityManager& entities,
    float dt
) {

    pruneAnimatorMap(slimeAnimators, entities);
    pruneAnimatorMap(summonerAnimators, entities);
    pruneAnimatorMap(skeletonAnimators, entities);
    pruneAnimatorMap(narutoBossAnimators, entities);
    pruneAnimatorMap(narutoCloneAnimators, entities);

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
        // SUMMONER
        // =====================

        auto* summoner =
            dynamic_cast<SummonerEnemy*>(
                ent.get()
            );

        if (summoner) {

            auto [iter, inserted] =
                summonerAnimators.try_emplace(
                    summoner
                );

            if (inserted) {
                iter->second.setupSummoner();
            }

            iter->second.update(
                *summoner,
                dt
            );

            iter->second.applyToEntity(
                *summoner,
                2.f
            );

            summoner->getSprite().setColor(
                sf::Color(200, 165, 255)
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

        // =====================
        // NARUTO BOSS
        // =====================

        auto* narutoBoss =
            dynamic_cast<NarutoBoss*>(
                ent.get()
            );

        if (narutoBoss) {

            auto [iter, inserted] =
                narutoBossAnimators.try_emplace(
                    narutoBoss
                );

            iter->second.update(
                *narutoBoss,
                dt
            );

            continue;
        }

        // =====================
        // NARUTO CLONE
        // =====================

        auto* narutoClone =
            dynamic_cast<NarutoCloneEnemy*>(
                ent.get()
            );

        if (narutoClone) {

            auto [iter, inserted] =
                narutoCloneAnimators.try_emplace(
                    narutoClone
                );

            iter->second.updateClone(
                *narutoClone,
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

        if (auto* narutoBoss = dynamic_cast<NarutoBoss*>(ent.get())) {
            if (narutoBoss->isHidden() &&
                !narutoBoss->isIntroSmokeActive() &&
                !narutoBoss->isSubstitutionReappearSmoke()) {
                continue;
            }
        }

        window.draw(
            ent->getSprite()
        );
    }
}
