#pragma once

#include <SFML/System/Vector2.hpp>
#include <cmath>

namespace MathUtils {

    inline float length(sf::Vector2f v) {
        return std::sqrt(v.x * v.x + v.y * v.y);
    }

    inline sf::Vector2f normalize(sf::Vector2f v) {
        float len = length(v);
        if (len < 0.0001f) return {0.f, 0.f};
        return v / len;
    }

    inline float distance(sf::Vector2f a, sf::Vector2f b) {
        return length(a - b);
    }

    inline sf::Vector2f directionTo(sf::Vector2f from, sf::Vector2f to) {
        return normalize(to - from);
    }
}
