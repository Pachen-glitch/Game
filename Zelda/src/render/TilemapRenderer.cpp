#include "TilemapRenderer.h"
#include "../core/Constants.h"
#include "../map/Map.h"

#include <SFML/Graphics/RectangleShape.hpp>

void TilemapRenderer::draw(sf::RenderWindow& window, const Map& map) {
    sf::RectangleShape tile;
    tile.setSize({
        static_cast<float>(Constants::TILE_SIZE),
        static_cast<float>(Constants::TILE_SIZE)
    });

    for (int y = 0; y < map.getHeight(); ++y) {
        for (int x = 0; x < map.getWidth(); ++x) {
            TileType t = map.getTile(x, y);
            if (t == TileType::FLOOR) {
                tile.setFillColor(sf::Color(72, 110, 72));
            } else if (t == TileType::DOOR || t == TileType::LOCKED_DOOR) {
                tile.setFillColor(sf::Color(140, 100, 60));
            } else {
                tile.setFillColor(sf::Color(45, 45, 55));
            }
            tile.setPosition(
                x * Constants::TILE_SIZE,
                y * Constants::TILE_SIZE
            );
            window.draw(tile);
        }
    }
}
