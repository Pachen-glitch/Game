#include "Movement.h"

void handleMovement(Player& player, char input) {
    switch (input) {
        case 'w':
            player.move(0, -1);
            break;
        case 's':
            player.move(0, 1);
            break;
        case 'a':
            player.move(-1, 0);
            break;
        case 'd':
            player.move(1, 0);
            break;
        case ' ':
            // ataque básico
            // por ahora solo mensaje
            break;
    }
}