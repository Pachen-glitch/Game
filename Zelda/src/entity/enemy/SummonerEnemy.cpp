#include "SummonerEnemy.h"
#include "SlimeEnemy.h"
#include "../base/EntityManager.h"
#include "../player/Player.h"
#include "../../utils/MathUtils.h"
#include "../../utils/AssetPaths.h"

SummonerEnemy::SummonerEnemy(sf::Vector2f pos)
    : Enemy(pos, EnemyKind::Summoner, 3, AssetPaths::getEnemySprite("summoner"))
{
    moveSpeed = 60.f;
    aggroRadius = 220.f;
}

void SummonerEnemy::think(Player& player, float dt, const Map& map) {
    Enemy::think(player, dt, map);

    summonCooldown -= dt;
    if (summonCooldown <= 0.f && entities && seesPlayer(player)) {
        entities->spawn<SlimeEnemy>(getPosition() + sf::Vector2f(32.f, 32.f));
        summonCooldown = 4.f;
        setAIState(EnemyState::CallAllies);
    }

    if (getAIState() == EnemyState::Chase &&
        MathUtils::distance(getPosition(), player.getPosition()) < 100.f) {
        sf::Vector2f away = MathUtils::directionTo(
            player.getPosition(), getPosition()
        );
        velocity = away * moveSpeed;
    }
}
