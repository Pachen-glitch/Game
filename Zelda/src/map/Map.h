#pragma once

#include <vector>



enum TileType {

    FLOOR,
    WALL
};





class Map {

private:

    std::vector<std::vector<TileType>> grid;

public:

    
    Map();

   

    TileType getTile(int x, int y);

    void setTile(
        int x,
        int y,
        TileType tile
    );

   
    int getWidth();

    int getHeight();
};