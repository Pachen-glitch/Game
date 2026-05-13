#pragma once
#include <vector>

enum TileType {
    EMPTY,
    WALL
};

class Map {
public:
    Map();

    TileType getTile(int x, int y);
    int getWidth();
    int getHeight();

private:
    std::vector<std::vector<TileType>> grid;
};