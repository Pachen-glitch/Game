#include <iostream>
#include <vector>
#include <unistd.h> // usleep
using namespace std;

const int WIDTH = 40;
const int HEIGHT = 15;

// Mapa base
vector<string> map = {
    "########################################",
    "#                                      #",
    "#                                      #",
    "#                                      #",
    "#                                      #",
    "#                                      #",
    "#                                      #",
    "#                                      #",
    "#                                      #",
    "#                                      #",
    "#                                      #",
    "#                                      #",
    "#                                      #",
    "#                                      #",
    "########################################"
};

// Función para limpiar pantalla
void clearScreen() {
    system("clear"); // Linux/Mac
    // system("cls"); // Windows
}

// Renderizado
void drawUI(int life, int score) {
    cout << "\n";
    cout << "  ZELDA ASCII - DUNGEON\n";
    cout << "  Vida: " << life << "   Score: " << score << "\n";
    cout << "\n";
}

// Dibujar mapa
void drawMap(int playerX, int playerY) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (i == playerY && j == playerX) {
                cout << "@"; // jugador
            } else {
                cout << map[i][j];
            }
        }
        cout << endl;
    }
}

// Loop base
int main() {
    int playerX = 20;
    int playerY = 7;

    int life = 3;
    int score = 0;

    while (true) {
        clearScreen();

        drawUI(life, score);
        drawMap(playerX, playerY);

        usleep(100000); // 0.1 segundos
    }

    return 0;
}