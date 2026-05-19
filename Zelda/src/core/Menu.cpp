#include <iostream>
#include <conio.h>

#include "Menu.h"
using namespace std;

void showStartScreen() {

    system("cls");

    cout << "########################################\n";
    cout << "#                                      #\n";
    cout << "#      THE LEGEND OF PACHEN            #\n";
    cout << "#                                      #\n";
    cout << "#              /\\                      #\n";
    cout << "#             /  \\                     #\n";
    cout << "#            /____\\                    #\n";
    cout << "#                                      #\n";
    cout << "#      PUSH START BUTTON (ENTER)       #\n";
    cout << "#                                      #\n";
    cout << "########################################\n";

    // WAITSAD
    while (true) {
        if (_kbhit()) {
            char key = _getch();
            if (key == 13) break; // ENTER
        }
    }
}