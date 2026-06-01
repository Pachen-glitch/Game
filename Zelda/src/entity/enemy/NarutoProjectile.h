#pragma once

#include "../base/Entity.h"

#include <SFML/System/Vector2.hpp>

class Player;

enum class NarutoProjectileType {
    Odama,
    RasenShuriken
};

// Boss projectiles — separate entities, not the boss sprite.
class NarutoProjectile : public Entity {
public:
    NarutoProjectile(
        sf::Vector2f pos,
        sf::Vector2f velocity,
        NarutoProjectileType type,
        float damage
    );

    void update(float dt) override;
    void onInteract(Player& player) override;

    NarutoProjectileType getProjectileType() const { return projType; }
    float getDamage() const { return damage; }
    float getKnockback() const { return knockback; }

private:
    sf::Vector2f velocity;
    NarutoProjectileType projType;
    float damage = 1.f;
    float knockback = 140.f;
    float lifetime = 4.f;
    float age = 0.f;
};
