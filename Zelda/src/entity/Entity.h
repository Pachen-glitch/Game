#pragma once

#include <string>

class Player;

class Entity {

protected:

    int x;
    int y;

    int width;
    int height;

    bool active;

    std::string spritePath;

public:

    Entity(
        int startX,
        int startY,
        int tileWidth,
        int tileHeight,
        std::string sprite
    );

    virtual ~Entity();


    // POSITION


    int getX();

    int getY();

    void setPosition(int newX, int newY);

  
    // SPRITES


    std::string getSprite();

  
    // STATE
  

    bool isActive();

    void deactivate();


    // INTERACTION
 

    virtual void onInteract(Player& player) = 0;


    // UPDATE
   

    virtual void update() = 0;
};