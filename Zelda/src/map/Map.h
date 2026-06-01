#pragma once

#include <vector>
#include <SFML/System/Vector2.hpp>

enum class TileType {
    FLOOR,
    WALL,
    DOOR,
    LOCKED_DOOR,
    OPENING,
    TREE,
    WATER,
    ROCK,
    BUSH,
    PIT,
    STATUE
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
    bool isWalkable(int x, int y) const;
    bool isDoor(int x, int y) const;
    sf::Vector2i worldToTile(sf::Vector2f pos) const;
    sf::Vector2f tileToWorld(int x, int y) const;

private:
    std::vector<std::vector<TileType>> grid;

    void initGrid(int w, int h, TileType fillType);
};
