#include "Input.h"
#include <conio.h>

char getInput() {
    if (_kbhit()) {
        return _getch();
    }
    return '\0';
}