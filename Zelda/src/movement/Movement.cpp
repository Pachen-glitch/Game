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

  

    if (map.getTile(newX, newY) == WALL) {
        return;
    }

   

    if (map.getTile(newX, newY) == COIN) {

        player.coins++;

        map.setTile(newX, newY, EMPTY);
    }

   

    if (map.getTile(newX, newY) == KEY) {

        player.keys++;

        map.setTile(newX, newY, EMPTY);
    }



    if (map.getTile(newX, newY) == HEART) {

        if (player.lives < 3)
            player.lives++;

        map.setTile(newX, newY, EMPTY);
    }

   

    if (map.getTile(newX, newY) == DOOR) {

        // luego harán cambio de sala
    }

  

    if (map.getTile(newX, newY) == ENEMY) {

        player.lives--;
    }

    

    player.x = newX;
    player.y = newY;
}