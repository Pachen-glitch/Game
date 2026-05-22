#pragma once

#include "EnemyState.h"

#include <SFML/System/Vector2.hpp>

class Enemy;
class Player;

// Shared AI helpers — aggro memory, patrol, chase.
class EnemyAI {
public:
    static void updatePatrol(Enemy& enemy, float dt);
    static void updateChase(Enemy& enemy, Player& player, float dt, float speed);
    static bool playerInRadius(sf::Vector2f a, sf::Vector2f b, float radius);
    static void faceTarget(Enemy& enemy, sf::Vector2f target);
};
