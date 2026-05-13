#pragma once

class Player {
public:
    int x, y;

    Player(int startX, int startY);

    void move(int dx, int dy);
};