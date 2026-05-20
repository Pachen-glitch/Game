#include <iostream>
#include <conio.h>

using namespace std;

void showPauseMenu(int enemiesKilled, int deaths, int coins) {

    system("cls");

    cout << "========================================\n";
    cout << "          THE LEGEND OF PACHEN         \n";
    cout << "========================================\n\n";

    cout << "             GAME PAUSED               \n\n";

    cout << "----------------------------------------\n";
    cout << "              CONTROLS                 \n";
    cout << "----------------------------------------\n";

    cout << "   W  -> Move Up\n";
    cout << "   S  -> Move Down\n";
    cout << "   A  -> Move Left\n";
    cout << "   D  -> Move Right\n";
    cout << "   F  -> Basic Attack\n";
    cout << " SPACE -> Special Attack\n";
    cout << " ESC -> Pause Menu\n\n";

    cout << "----------------------------------------\n";
    cout << "             STATISTICS                \n";
    cout << "----------------------------------------\n";

    cout << "   Enemies defeated : " << enemiesKilled << endl;
    cout << "   Deaths           : " << deaths << endl;
    cout << "   Coins collected  : " << coins << endl;

    cout << "\n----------------------------------------\n";
    cout << "      PRESS ENTER TO CONTINUE          \n";
    cout << "----------------------------------------\n";

    while (true) {

        if (_kbhit()) {

            char key = _getch();

            if (key == 13 || key == 27) {
                break;
            }
        }
    }
}