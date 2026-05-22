#include "core/Game.h"
#include "ui/Menu.h"

#include <cstdio>
#include <iostream>

int main() {
    // Log de arranque — si existe tras ejecutar, main() si llego a correr.
    {
        FILE* boot = fopen("zelda_boot.log", "w");
        if (boot) {
            fprintf(boot, "main() started\n");
            fclose(boot);
        }
    }

    showMenu();

    std::cout << std::endl;
    std::cout << "Iniciando ventana del juego..." << std::endl;
    std::cout << "(Si no ves nada, revisa que no quede detras de otras ventanas)" << std::endl;
    std::cout << std::endl;

    runGame();

    return 0;
}