#pragma once

#include <vector>

enum class TileType {
    FLOOR,
    WALL,
    DOOR,
    LOCKED_DOOR
};

class Map {
public:
    Map();
    explicit Map(int width, int height);

    TileType getTile(int x, int y) const;
    void setTile(int x, int y, TileType tile);

    int getWidth() const;
    int getHeight() const;

    void fill(TileType tile);

private:
    std::vector<std::vector<TileType>> grid;
    void initGrid(int w, int h, TileType fillType);
};
