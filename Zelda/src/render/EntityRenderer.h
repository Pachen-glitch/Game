#pragma once

#include "../entity/base/EntityManager.h"

#include "../entity/enemy/SlimeEnemy.h"
#include "../entity/enemy/SkeletonEnemy.h"
#include "../entity/enemy/SummonerEnemy.h"
#include "../entity/enemy/NarutoBoss.h"
#include "../entity/enemy/NarutoCloneEnemy.h"
#include "../entity/enemy/BatEnemy.h"

#include "../render/EnemyAnimator.h"
#include "../render/SkeletonRenderer.h"
#include "../render/NarutoRenderer.h"



#include <SFML/Graphics/RenderWindow.hpp>

#include <unordered_map>

class Player;

class EntityRenderer {
public:

    void update(
        EntityManager& entities,
        float dt
    );

    void draw(
        sf::RenderWindow& window,
        const Player& player,
        EntityManager& entities
    );

private:

    std::unordered_map<
        SlimeEnemy*,
        EnemyAnimator
    > slimeAnimators;

    std::unordered_map<
        BatEnemy*,
        EnemyAnimator
    > batAnimators;

    std::unordered_map<
        SummonerEnemy*,
        EnemyAnimator
    > summonerAnimators;

    std::unordered_map<
        SkeletonEnemy*,
        SkeletonRenderer
    > skeletonAnimators;

    std::unordered_map<
        NarutoBoss*,
        NarutoRenderer
    > narutoBossAnimators;

    std::unordered_map<
        NarutoCloneEnemy*,
        NarutoRenderer
    > narutoCloneAnimators;
};