#include "HUD.h"
#include "../entity/player/Player.h"
#include "../utils/FontLoader.h"

#include <SFML/Graphics/Text.hpp>

HUD::HUD() {
    fontLoaded = FontLoader::load(font);
}

void HUD::draw(sf::RenderWindow& window, const Player& player) {
    if (!fontLoaded) return;

    sf::Text hearts;
    hearts.setFont(font);
    hearts.setCharacterSize(22);
    hearts.setFillColor(sf::Color::Red);
    hearts.setPosition(16.f, 12.f);
    hearts.setString("Hearts: " + std::to_string(player.getStats().hearts) +
        "/" + std::to_string(player.getStats().maxHearts));

    sf::Text rupees;
    rupees.setFont(font);
    rupees.setCharacterSize(22);
    rupees.setFillColor(sf::Color(40, 200, 80));
    rupees.setPosition(16.f, 40.f);
    rupees.setString("Rupees: " + std::to_string(player.getStats().rupees));

    sf::Text keys;
    keys.setFont(font);
    keys.setCharacterSize(22);
    keys.setFillColor(sf::Color::Yellow);
    keys.setPosition(16.f, 68.f);
    keys.setString("Keys: " + std::to_string(player.getStats().keys));

    window.setView(window.getDefaultView());
    window.draw(hearts);
    window.draw(rupees);
    window.draw(keys);
}
