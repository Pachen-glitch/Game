#include "CollisionSystem.h"
#include "../core/Constants.h"
#include "../map/Map.h"

#include <SFML/Graphics/Rect.hpp>
// Devuelve si el rectangulo colisiona con un wall
static bool rectHitsWall(const Map& map, sf::FloatRect box) { // Devuelve si el rectangulo colisiona con un wall
    if (CollisionSystem::isWall(map, {box.left, box.top})) return true;
    if (CollisionSystem::isWall(map, {box.left + box.width - 1.f, box.top})) return true;
    if (CollisionSystem::isWall(map, {box.left, box.top + box.height - 1.f})) return true;
    if (CollisionSystem::isWall(map, {box.left + box.width - 1.f, box.top + box.height - 1.f})) return true;
    return false;
}
// Devuelve si el vector de mundo colisiona con un wall
bool CollisionSystem::isWall(const Map& map, sf::Vector2f worldPos) {
    sf::Vector2i tile = map.worldToTile(worldPos); // Convierte la posicion del mundo a la posicion de la grid  
    return !map.isWalkable(tile.x, tile.y);
}
// Resuelve la movimiento con colisiones
sf::Vector2f CollisionSystem::resolveMovement(
    const Map& map,
    sf::Vector2f from,
    sf::Vector2f to,
    sf::Vector2f size
) {
    sf::Vector2f result = from; // Resultado de la movimiento

    sf::FloatRect boxX(to.x, from.y, size.x, size.y);
    if (!rectHitsWall(map, boxX)) {
        result.x = to.x;
    }

    sf::FloatRect boxY(result.x, to.y, size.x, size.y);
    if (!rectHitsWall(map, boxY)) {
        result.y = to.y;
    }

    return result;
}
