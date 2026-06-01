#include <iostream>
//#include <windows.h>
#include <SFML/System.hpp>

using namespace std;

void showInstructions() {

    system("cls");

    cout << "=====================================" << endl;
    cout << "           INSTRUCCIONES             " << endl;
    cout << "=====================================" << endl;
    cout << endl;

    cout << "CONTROLES:" << endl;
    cout << endl;

    cout << "W -> Mover arriba" << endl;
    cout << "S -> Mover abajo" << endl;
    cout << "A -> Mover izquierda" << endl;
    cout << "D -> Mover derecha" << endl;
    cout << "ESC -> Salir del juego" << endl;

    cout << endl;

    cout << "OBJETIVO:" << endl;
    cout << "Explorar el mapa y evitar enemigos." << endl;

    cout << endl;
    system("pause");
}

void showMenu() {

    bool inMenu = true;

    while (inMenu) {

        system("cls");

        cout << "=====================================" << endl;
        cout << "          The Legend of Pachen           " << endl;
        cout << "=====================================" << endl;
        cout << endl;

        cout << "      /\\                            " << endl;
        cout << "     /  \\\\                           " << endl;
        cout << "    /----\\\\                          " << endl;
        cout << "   /      \\\\                         " << endl;
        cout << endl;

        cout << "1. Iniciar Juego" << endl;
        cout << "2. Instrucciones" << endl;
        cout << "3. Salir" << endl;

        cout << endl;
        cout << "Seleccione una opcion: ";

        char option;
        cin >> option;

        switch (option) {

        case '1':
            inMenu = false;
            break;

        case '2':
            showInstructions();
            break;

        case '3':
            exit(0);
            break;

        default:
            cout << endl;
            cout << "Opcion invalida." << endl;
            sf::sleep(sf::milliseconds(1000));
        }
    }
}
