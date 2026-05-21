#include "PlayerMovement.h"


// PLAYER MOVEMENT


void PlayerMovement::move(
    Player& player,
    char input,
    Map& map
) {

    int newX = player.getX();
    int newY = player.getY();

    switch (input) {

        case 'w':

            newY--;

            player.setDirection(UP);

            break;

        case 's':

            newY++;

            player.setDirection(DOWN);

            break;

        case 'a':

            newX--;

            player.setDirection(LEFT);

            break;

        case 'd':

            newX++;

            player.setDirection(RIGHT);

            break;
    }

    // COLLISION


    if (
        map.getTile(newX, newY) == WALL
    ) {
        return;
    }

    // APPLY MOVEMENT
   

    player.setPosition(
        newX,
        newY
    );
}
