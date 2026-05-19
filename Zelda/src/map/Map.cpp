#include "Map.h"

Map::Map() {
    grid = {
        {WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL},
        {WALL,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,WALL},
        {WALL,EMPTY,WALL,WALL,EMPTY,WALL,WALL,EMPTY,EMPTY,WALL},
        {WALL,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,EMPTY,WALL},
        {WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL,WALL}
    };
}

TileType Map::getTile(int x, int y) {
    return grid[y][x];
}

int Map::getWidth() {
    return grid[0].size();
}

int Map::getHeight() {
    return grid.size();
}