#include "Entity.h"
#include "../../render/TextureCache.h"

Entity::Entity(
    sf::Vector2f startPos,
    sf::Vector2f size,
    const std::string& path,
    EntityType type
)
    : position(startPos)
    , size(size)
    , spritePath(path)
    , entityType(type)
{
    const sf::Texture& tex = TextureCache::instance().get(spritePath);
    sprite.setTexture(tex, true);
    sprite.setPosition(position);
    sprite.setScale(
        size.x / static_cast<float>(tex.getSize().x),
        size.y / static_cast<float>(tex.getSize().y)
    );
}

Entity::~Entity() = default;

sf::Vector2f Entity::getPosition() const { return position; }

void Entity::setPosition(sf::Vector2f pos) {
    position = pos;
    sprite.setPosition(position);
}

sf::Vector2f Entity::getSize() const { return size; }

sf::FloatRect Entity::getBounds() const {
    return sf::FloatRect(position.x, position.y, size.x, size.y);
}

sf::Sprite& Entity::getSprite() { return sprite; }
const sf::Sprite& Entity::getSprite() const { return sprite; }

bool Entity::isActive() const { return active; }
void Entity::deactivate() { active = false; }
void Entity::activate() { active = true; }

EntityType Entity::getType() const { return entityType; }
std::string Entity::getSpritePath() const { return spritePath; }
