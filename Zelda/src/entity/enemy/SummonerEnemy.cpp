#include "SummonerEnemy.h"
#include "SlimeEnemy.h"
#include "../base/EntityManager.h"
#include "../player/Player.h"
#include "../../utils/MathUtils.h"
#include "../../utils/AssetPaths.h"

SummonerEnemy::SummonerEnemy(sf::Vector2f pos)
    : Enemy(pos, EnemyKind::Summoner, 3, AssetPaths::getEnemySprite("summoner", "idle", 1))
{
    moveSpeed = 60.f;
    aggroRadius = 220.f;
}

void SummonerEnemy::think(Player& player, float dt, const Map& map) { // Piensa el enemigo
    Enemy::think(player, dt, map);

    summonCooldown -= dt;
    if (summonCooldown <= 0.f && entities && seesPlayer(player)) { // Spawna un slime cuando el enemigo ve al jugador
        entities->spawn<SlimeEnemy>(getPosition() + sf::Vector2f(32.f, 32.f));
        summonCooldown = 4.f;
        setAIState(EnemyState::CallAllies);
    }

    if (getAIState() == EnemyState::Chase && // Si el enemigo esta en estado de chase y la distancia entre el enemigo y el jugador es menor a 100, se mueve hacia el jugador
        MathUtils::distance(getPosition(), player.getPosition()) < 100.f) {
        sf::Vector2f away = MathUtils::directionTo( // Calcula la direccion hacia el jugador
            player.getPosition(), getPosition()
        );
        velocity = away * moveSpeed; // Aplica la velocidad al enemigo
    }
}
