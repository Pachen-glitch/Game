#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Vector2.hpp>
#include <string>

class Player;

enum class EntityType {
    Player,
    Enemy,
    Coin,
    Chest,
    Key,
    Heart,
    Door,
    Shopkeeper,
    Projectile,
    Generic
};

// Base for all world objects — position, sprite, lifecycle, interaction hook.
class Entity {
public:
    Entity(
        sf::Vector2f startPos,
        sf::Vector2f size,
        const std::string& spritePath,
        EntityType type = EntityType::Generic
    );

    virtual ~Entity();

    virtual void update(float deltaTime) = 0;
    virtual void onInteract(Player& player) = 0;

    sf::Vector2f getPosition() const;
    void setPosition(sf::Vector2f pos);
    sf::Vector2f getSize() const;
    sf::FloatRect getBounds() const;

    sf::Sprite& getSprite();
    const sf::Sprite& getSprite() const;

    bool isActive() const;
    void deactivate();
    void activate();

    EntityType getType() const;
    std::string getSpritePath() const;

protected:
    sf::Vector2f position;
    sf::Vector2f size;
    sf::Sprite sprite;
    std::string spritePath;
    bool active = true;
    EntityType entityType;
};
