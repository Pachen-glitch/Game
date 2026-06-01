#include "Input.h"
#include <conio.h>

char getInput() { // Obtiene la entrada del teclado
    if (_kbhit()) {
        return _getch(); // Devuelve el caracter presionado
    }
    return '\0'; // Devuelve un caracter nulo
}