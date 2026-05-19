#include "Movement.h"

void handleMovement(Player& player, char input, Map& map)
{
    int newX = player.x;
    int newY = player.y;

    switch (input) {

        case 'w':
            newY--;
            break;

        case 's':
            newY++;
            break;

        case 'a':
            newX--;
            break;

        case 'd':
            newX++;
            break;
    }

    // colisiones básicas
    if (map.getTile(newX, newY) != WALL) {

        player.x = newX;
        player.y = newY;
    }
}