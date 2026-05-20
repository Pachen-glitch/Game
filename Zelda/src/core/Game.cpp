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


void render(Player& player, Map& map)
{
    for (int y = 0; y < map.getHeight(); y++) {

        for (int x = 0; x < map.getWidth(); x++) {

            // PLAYER
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
void drawUI(Player& player) {

    cout << "============================\n";
    cout << "THE LEGEND OF PACHEN\n";
    cout << "============================\n";

    cout << "Lives: ";

    for (int i = 0; i < 3; i++) {

        if (i < player.lives)
            cout << "<3 ";
        else
            cout << "X ";
    }

    cout << "\n";

    cout << "Coins: " << player.coins << "\n";
    cout << "Keys : " << player.keys << "\n";

    cout << "============================\n\n";
}

// Configuración de la consola
void setWindowSize() {
    system("mode con: cols=50 lines=20");
}
void hideCursor() {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &info);
}

// LOOP PRINCIPAL
void runGame() {

    showMenu();

    system("cls");

    Player player(10, 5);

    Map map;

    hideCursor();

    setWindowSize();

    bool running = true;

    while (running) {

        char input = getInput();

        if (input == 27) {
            running = false;
        }

        if (input != '\0') {

            handleMovement(player, input, map);
        }

        clearScreen();

        drawUI(player);

        render(player, map);

        Sleep(50);
    }
}