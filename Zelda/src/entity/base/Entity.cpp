#include "Entity.h"

Entity::Entity(
    int startX,
    int startY,
    int tileWidth,
    int tileHeight,
    std::string sprite
) {

    x = startX;
    y = startY;

    width = tileWidth;
    height = tileHeight;

    spritePath = sprite;

    active = true;
}

Entity::~Entity() {}

int Entity::getX() {

    return x;
}

int Entity::getY() {

    return y;
}

void Entity::setPosition(int newX, int newY) {

    x = newX;
    y = newY;
}

std::string Entity::getSprite() {

    return spritePath;
}

bool Entity::isActive() {

    return active;
}

void Entity::deactivate() {

    active = false;
}