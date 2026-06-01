#include "NarutoProjectile.h"

#include "../player/Player.h"
#include "../../render/TextureCache.h"
#include "../../utils/AssetPaths.h"

NarutoProjectile::NarutoProjectile(
    sf::Vector2f pos,
    sf::Vector2f vel,
    NarutoProjectileType type,
    float dmg
)
    : Entity(pos, {48.f, 48.f}, "", EntityType::Projectile)
    , velocity(vel)
    , projType(type)
    , damage(dmg)
{
    if (type == NarutoProjectileType::Odama) {
        size = {72.f, 72.f};
        lifetime = 5.f;
        knockback = 180.f;
        spritePath = AssetPaths::resolveFirst({
            "sprites/enemies/naruto/odama_br1.png",
            "sprites/enemies/naruto/odama_l1.png"
        });
    } else {
        size = {56.f, 56.f};
        lifetime = 3.5f;
        knockback = 200.f;
        spritePath = AssetPaths::resolveFirst({
            "sprites/enemies/naruto/rasenShuriken_l1.png",
            "sprites/enemies/naruto/rasenShuriken_r1.png"
        });
    }

    if (!spritePath.empty()) {
        const sf::Texture& tex = TextureCache::instance().get(spritePath);
        sprite.setTexture(tex, true);
        sprite.setScale(
            size.x / static_cast<float>(tex.getSize().x),
            size.y / static_cast<float>(tex.getSize().y)
        );
    }
}

void NarutoProjectile::update(float dt) {
    age += dt;
    if (age >= lifetime) {
        deactivate();
        return;
    }
    setPosition(getPosition() + velocity * dt);
}

void NarutoProjectile::onInteract(Player&) {}
