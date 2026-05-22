#include "DamageNumbers.h"
#include "../utils/FontLoader.h"

#include <SFML/Graphics/Text.hpp>

#include <algorithm>

DamageNumbers::DamageNumbers() {
    fontLoaded = FontLoader::load(font);
}

void DamageNumbers::spawn(sf::Vector2f pos, int amount, bool crit) {
    FloatingText ft;
    ft.pos = pos;
    ft.text = std::to_string(amount);
    ft.color = crit ? sf::Color::Yellow : sf::Color::White;
    texts.push_back(ft);
}

void DamageNumbers::update(float dt) {
    for (auto& t : texts) {
        t.age += dt;
        t.pos.y -= 30.f * dt;
    }
    texts.erase(
        std::remove_if(texts.begin(), texts.end(),
            [](const FloatingText& t) { return t.age >= t.lifetime; }),
        texts.end()
    );
}

void DamageNumbers::draw(sf::RenderWindow& window) {
    if (!fontLoaded) return;

    for (const auto& t : texts) {
        sf::Text txt;
        txt.setFont(font);
        txt.setString(t.text);
        txt.setCharacterSize(20);
        txt.setFillColor(t.color);
        txt.setPosition(t.pos);
        window.draw(txt);
    }
}
