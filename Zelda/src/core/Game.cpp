#include <iostream>
#include <windows.h> // Sleep
#include "../input/Input.h"
#include "../movement/Movement.h"
#include "../entities/Player.h"
#include "../ui/Menu.h"

using namespace std;

// Limpiar consola
void clearScreen() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord = { 0, 0 };
    SetConsoleCursorPosition(hConsole, coord);
}


void render(Player& player) {

    const int WIDTH = 20;
    const int HEIGHT = 10;

    for (int y = 0; y < HEIGHT; y++) {

        for (int x = 0; x < WIDTH; x++) {

            if (x == player.x && y == player.y) {
                cout << "@";
            }

            else if (x == 5 && y == 2) {
                cout << "E";
            }

            else if (x == 7 && y == 1) {
                cout << "$";
            }

            else if (x == 8 && y == 3) {
                cout << "K";
            }

            else if (x == 2 && y == 3) {
                cout << "H";
            }

            else if (y == 0 || y == HEIGHT - 1 || x == 0 || x == WIDTH - 1) {
                cout << "#";
            }

            else {
                cout << " ";
            }

        }

        cout << endl;
    }
}

// LOOP PRINCIPAL
void runGame() {

    showMenu();

    system("cls");

    Player player(10, 5);

    bool running = true;

    while (running) {

        // 1. INPUT
        char input = getInput();

        // salir con ESC
        if (input == 27) {
            running = false;
        }

        if (input != '\0') {
            handleMovement(player, input);
        }

        // 2. UPDATE


        // 3. RENDER
        clearScreen();
        render(player);

        // 4. CONTROL DE TIEMPO
        Sleep(50); // 50 ms
    }
}