#include "ShopUI.h"
#include "../entity/player/Player.h"
#include "../utils/FontLoader.h"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/Keyboard.hpp>

ShopUI::ShopUI() {
    fontLoaded = FontLoader::load(font);
}

void ShopUI::draw(sf::RenderWindow& window, const Shop& shop, const Player& player) {
    if (!fontLoaded) return;

    sf::RectangleShape panel;
    panel.setSize({400.f, 300.f});
    panel.setPosition(440.f, 210.f);
    panel.setFillColor(sf::Color(20, 20, 40, 230));
    window.draw(panel);

    sf::Text title;
    title.setFont(font);
    title.setString("SHOP");
    title.setCharacterSize(28);
    title.setPosition(560.f, 220.f);
    window.draw(title);

    int y = 270;
    int i = 0;
    for (const auto& item : shop.getItems()) {
        sf::Text line;
        line.setFont(font);
        line.setCharacterSize(18);
        line.setPosition(460.f, static_cast<float>(y));
        line.setString(
            std::to_string(i) + ": " + item.name +
            " - " + std::to_string(item.cost) + " R"
        );
        window.draw(line);
        y += 28;
        if (++i >= 5) break;
    }

    (void)player;
}

void ShopUI::handleInput(Shop& shop, Player& player) {
    using namespace sf;
    if (Keyboard::isKeyPressed(Keyboard::Num1)) shop.buy(player, 0);
    if (Keyboard::isKeyPressed(Keyboard::Num2)) shop.buy(player, 1);
    if (Keyboard::isKeyPressed(Keyboard::Num3)) shop.buy(player, 2);
}
