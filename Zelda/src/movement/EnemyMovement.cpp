#include "EnemyMovement.h"
#include "CollisionSystem.h"
#include "../core/Constants.h"
#include "../map/Map.h"

sf::Vector2f EnemyMovement::moveWithCollision(
    const Map& map,
    sf::Vector2f from,
    sf::Vector2f velocity,
    float dt,
    sf::Vector2f size
) {
    sf::Vector2f to = from + velocity * dt;
    sf::Vector2f resolved = CollisionSystem::resolveMovement(map, from, to, size);
    return clampToRoom(resolved, size, map);
}

sf::Vector2f EnemyMovement::clampToRoom(
    sf::Vector2f pos,
    sf::Vector2f size,
    const Map& map
) {
    float margin = static_cast<float>(Constants::TILE_SIZE);
    float maxX = (map.getWidth() - 1) * Constants::TILE_SIZE - size.x - margin;
    float maxY = (map.getHeight() - 1) * Constants::TILE_SIZE - size.y - margin;

    if (pos.x < margin) pos.x = margin;
    if (pos.y < margin) pos.y = margin;
    if (pos.x > maxX) pos.x = maxX;
    if (pos.y > maxY) pos.y = maxY;
    return pos;
}
