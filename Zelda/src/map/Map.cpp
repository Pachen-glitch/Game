#include "Map.h"
#include "../core/Constants.h"
// Constructor de Map
Map::Map() {
    initGrid(Constants::ROOM_TILES_W, Constants::ROOM_TILES_H, TileType::FLOOR);
}
// Constructor de Map
Map::Map(int width, int height) {
    initGrid(width, height, TileType::FLOOR);
}
// Inicializa la grid del mapa
void Map::initGrid(int w, int h, TileType fillType) {
    grid.assign(h, std::vector<TileType>(w, fillType));
}
// Devuelve el tipo de tile en la posicion x, y
TileType Map::getTile(int x, int y) const {
    if (x < 0 || y < 0 || y >= static_cast<int>(grid.size()) ||
        x >= static_cast<int>(grid[0].size())) {
        return TileType::WALL;
    }
    return grid[y][x];
}
// Establece el tipo de tile en la posicion x, y
void Map::setTile(int x, int y, TileType tile) {
    if (x < 0 || y < 0 || y >= static_cast<int>(grid.size()) ||
        x >= static_cast<int>(grid[0].size())) {
        return;
    }
    grid[y][x] = tile;
}
// Devuelve el ancho del mapa
int Map::getWidth() const {
    return grid.empty() ? 0 : static_cast<int>(grid[0].size());
}
// Devuelve el alto del mapa
int Map::getHeight() const {
    return static_cast<int>(grid.size());
}
// Rellena el mapa con un tipo de tile
void Map::fill(TileType tile) {
    for (auto& row : grid) {
        for (auto& cell : row) cell = tile;
    }
}
// Devuelve si el tile en la posicion x, y es walkable
bool Map::isWalkable(int x, int y) const {

    TileType tile = getTile(x, y);

    switch (tile) {

        case TileType::FLOOR:
        case TileType::DOOR:
        case TileType::OPENING:
            return true;

        default:
            return false;
    }
}
// Devuelve si el tile en la posicion x, y es una puerta
bool Map::isDoor(int x, int y) const {
    TileType tile = getTile(x, y);

    return tile == TileType::DOOR ||
           tile == TileType::LOCKED_DOOR ||
           tile == TileType::OPENING;
}

sf::Vector2i Map::worldToTile(sf::Vector2f pos) const { // Convierte la posicion del mundo a la posicion de la grid
    return {
        static_cast<int>(pos.x) / Constants::TILE_SIZE,
        static_cast<int>(pos.y) / Constants::TILE_SIZE
    };
}

sf::Vector2f Map::tileToWorld(int x, int y) const { // Convierte la posicion de la grid a la posicion del mundo
    return {
        static_cast<float>(x * Constants::TILE_SIZE),
        static_cast<float>(y * Constants::TILE_SIZE)
    };
}