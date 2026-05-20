#pragma once
#include <vector>

enum TileType {
    EMPTY,
    WALL,
    DOOR,
    KEY,
    HEART,
    COIN,
    ENEMY
};

class Map {
public:
    Map();

    TileType getTile(int x, int y);
    void setTile(int x, int y, TileType tile);

    int getWidth();
    int getHeight();

private:
    std::vector<std::vector<TileType>> grid;
};