#pragma once

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>
#include <string>
#include <vector>

struct FloatingText {
    sf::Vector2f pos;
    std::string text;
    float lifetime = 0.8f;
    float age = 0.f;
    sf::Color color = sf::Color::White;
};

class DamageNumbers {
public:
    DamageNumbers();
    void spawn(sf::Vector2f pos, int amount, bool crit = false);
    void update(float dt);
    void draw(sf::RenderWindow& window);

private:
    std::vector<FloatingText> texts;
    sf::Font font;
    bool fontLoaded = false;
};
