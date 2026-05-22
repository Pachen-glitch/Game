#include "SummonerEnemy.h"
#include "SlimeEnemy.h"
#include "../base/EntityManager.h"
#include "../player/Player.h"
#include "../../utils/MathUtils.h"

SummonerEnemy::SummonerEnemy(sf::Vector2f pos)
    : Enemy(pos, EnemyKind::Summoner, 3,
        "assets/sprites/enemies/summoner/summoner_idle.png")
{
    moveSpeed = 60.f;
    aggroRadius = 220.f;
}

void SummonerEnemy::think(Player& player, float dt) {
    Enemy::think(player, dt);

    summonCooldown -= dt;
    if (summonCooldown <= 0.f && entities && seesPlayer(player)) {
        entities->spawn<SlimeEnemy>(getPosition() + sf::Vector2f(32.f, 32.f));
        summonCooldown = 4.f;
        setAIState(EnemyState::CallAllies);
    }

    if (getAIState() == EnemyState::Chase &&
        MathUtils::distance(getPosition(), player.getPosition()) < 100.f) {
        // keep distance
        sf::Vector2f away = MathUtils::directionTo(
            player.getPosition(), getPosition()
        );
        velocity = away * moveSpeed;
    }
}
