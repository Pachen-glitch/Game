#pragma once

class Player {
public:
    int x, y;
    int lives;

    Player(int startX, int startY);

    void move(int dx, int dy);
};