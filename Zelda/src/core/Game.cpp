#include <iostream>
#include <windows.h> // Sleep
#include "../input/Input.h"
#include "../movement/Movement.h"
#include "../entities/Player.h"
#include "../map/Map.h"
using namespace std;

// Limpiar consola
void clearScreen() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord = {0, 0};
    SetConsoleCursorPosition(hConsole, coord);
}

// Dibujar mapa simple + jugador
void render(Player& player, Map& map)
{
    clearScreen();

    for (int y = 0; y < map.getHeight(); y++) {

        for (int x = 0; x < map.getWidth(); x++) {

            // Jugador
            if (x == player.x && y == player.y) {
                cout << "@";
            }

            else {

                TileType tile = map.getTile(x, y);

                switch (tile) {

                    case WALL:
                        cout << "#";
                        break;

                    case EMPTY:
                        cout << ".";
                        break;

                    case DOOR:
                        cout << "D";
                        break;

                    case KEY:
                        cout << "K";
                        break;

                    case HEART:
                        cout << "H";
                        break;

                    case COIN:
                        cout << "$";
                        break;

                    case ENEMY:
                        cout << "E";
                        break;
                }
            }
        }

        cout << endl;
    }
}

// LOOP PRINCIPAL
void runGame() {
    Player player(10, 5);
    Map map;

    bool running = true;

    while (running) {

        // 1. INPUT
        char input = getInput();

        // salir con ESC
        if (input == 27) {
            running = false;
        }
        if (input != '\0') {
            handleMovement(player, input, map);
}
        // 2. UPDATE
        

        // 3. RENDER
        clearScreen();
        render(player, map);

        // 4. CONTROL DE TIEMPO
        Sleep(50); // 50 ms
    }
}