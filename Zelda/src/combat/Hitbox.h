#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>

enum class HitboxKind {
    Hurt,
    Hit
};

struct Hitbox {
    sf::FloatRect rect;
    HitboxKind kind = HitboxKind::Hit;
    float damage = 1.f;
    sf::Vector2f knockback;
    float lifetime = 0.2f;
    float age = 0.f;
    bool fromPlayer = false;
    int ownerId = -1;

    bool isExpired() const { return age >= lifetime; }
    void tick(float dt) { age += dt; }
};
