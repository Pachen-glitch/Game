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

    sf::RectangleShape panel;// Rectangulo para el panel
    panel.setSize({400.f, 300.f});
    panel.setPosition(440.f, 210.f);// Posicion del panel
    panel.setFillColor(sf::Color(20, 20, 40, 230));// Color del panel
    window.draw(panel);// Dibuja el panel

    sf::Text title;// Texto para el titulo
    title.setFont(font);// Fuente del titulo
    title.setString("SHOP");
    title.setCharacterSize(28);// Tamaño del titulo
    title.setPosition(560.f, 220.f);// Posicion del titulo
    window.draw(title);// Dibuja el titulo
    sf::Text money;// Texto para el dinero
    money.setFont(font);// Fuente del dinero
    money.setCharacterSize(20);// Tamaño del dinero
    money.setPosition(460.f, 245.f);
    money.setString(
        "Rupees: " +
        std::to_string(player.getCoins())// String del dinero
    );
    window.draw(money);

    int y = 270;
    int i = 0;
    for (const auto& item : shop.getItems()) {
        sf::Text line;
        line.setFont(font);
        line.setCharacterSize(18);
        line.setPosition(460.f, static_cast<float>(y));
        line.setString(
            std::to_string(i + 1) + ": " + item.name +
            " - " + std::to_string(item.cost) + " R"
        );
        window.draw(line);
        y += 28;// Incrementa la posicion y
        if (++i >= 5) break;
    }

    (void)player;

    sf::Text controls;// Texto para los controles
    controls.setFont(font);
    controls.setCharacterSize(18);
    controls.setString(
        "1-5 Buy   |   ESC Exit"
    );
    controls.setPosition(430.f, 620.f);
    controls.setFillColor(sf::Color::White);
    window.draw(controls);
}

void ShopUI::handleInput(Shop& shop,Player& player) {
    static bool pressed = false;
    if (!pressed) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) {
            shop.buy(player, 0);
            pressed = true;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)) {
            shop.buy(player, 1);
            pressed = true;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3)) {
            shop.buy(player, 2);
            pressed = true;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num4)) {
            shop.buy(player, 3);
            pressed = true;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num5)) {
            shop.buy(player, 4);
            pressed = true;
        }
    }
    if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Num1) &&
        !sf::Keyboard::isKeyPressed(sf::Keyboard::Num2) &&
        !sf::Keyboard::isKeyPressed(sf::Keyboard::Num3) &&
        !sf::Keyboard::isKeyPressed(sf::Keyboard::Num4) &&
        !sf::Keyboard::isKeyPressed(sf::Keyboard::Num5))
         {pressed = false;
        }
}