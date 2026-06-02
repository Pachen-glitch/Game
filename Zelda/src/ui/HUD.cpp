#include "HUD.h"
#include "../entity/player/Player.h"
#include "../utils/FontLoader.h"

#include <SFML/Graphics/Text.hpp>

HUD::HUD() {
    fontLoaded = FontLoader::load(font);
}

void HUD::draw(sf::RenderWindow& window, const Player& player) {
    if (!fontLoaded) return;

    auto fmt = [](float v) {
        std::string s = std::to_string(v);
        auto dot = s.find('.');
        if (dot != std::string::npos && dot + 2 < s.size()) {
            s = s.substr(0, dot + 2);
        }
        return s;
    };

    sf::Text hearts;
    hearts.setFont(font);
    hearts.setCharacterSize(22);
    hearts.setFillColor(sf::Color::Red);
    hearts.setPosition(16.f, 12.f);
    hearts.setString(
        "Hearts: " + fmt(player.getStats().hearts) +
        "/" + fmt(player.getPersistedMaxHearts())
    );

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

    float statusY = 96.f;
    if (player.isShieldActive()) {
        sf::Text shield;
        shield.setFont(font);
        shield.setCharacterSize(18);
        shield.setFillColor(sf::Color(120, 190, 255));
        shield.setPosition(16.f, statusY);
        shield.setString("[SHIELD]");
        window.draw(shield);
        statusY += 24.f;
    }
    if (player.isBerserkActive()) {
        sf::Text berserk;
        berserk.setFont(font);
        berserk.setCharacterSize(18);
        berserk.setFillColor(sf::Color(255, 120, 60));
        berserk.setPosition(16.f, statusY);
        berserk.setString("[BERSERK]");
        window.draw(berserk);
        statusY += 24.f;
    }
    if (Player::isDebugGodMode()) {
        sf::Text godMode;
        godMode.setFont(font);
        godMode.setCharacterSize(18);
        godMode.setFillColor(sf::Color(255, 255, 120));
        godMode.setPosition(16.f, statusY);
        godMode.setString("[GOD MODE]");
        window.draw(godMode);
    }
}
