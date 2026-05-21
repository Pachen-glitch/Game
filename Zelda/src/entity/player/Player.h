#pragma once

#include "../base/Entity.h"

enum Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

class Player : public Entity {

private:

    int lives;
    int coins;
    int keys;

    bool shieldActive;

    Direction direction;

    bool attacking;
    bool spinning;

public:

    Player(int startX, int startY);

    
    // MOVEMENT
  

    void move(int dx, int dy);

    void setDirection(Direction dir);

    Direction getDirection();

   
    // SIGNALS / evenbysd
  

    void addCoin();

    void addKey();

    void heal(int amount);

    void damage(int amount);

   
    // ATTACKS
   

    void swordAttack();

    void spinAttack();

    bool isAttacking();

    bool isSpinning();

    
    // SHIELD
    
    void activateShield();

    void deactivateShield();

    bool hasShield();
    void onInteract(Player& player) override;
  
    // GETTERS
  

    int getLives();

    int getCoins();

    int getKeys();

  
    // UPDATE


    void update() override;
};