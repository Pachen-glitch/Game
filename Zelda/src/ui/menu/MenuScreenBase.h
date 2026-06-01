#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Vector2.hpp>

#include <string>
#include <vector>

// Shared full-screen background + text layout for sub-menu screens.
class MenuScreenBase {
public:
    MenuScreenBase();

    void setBackgroundPath(const std::string& assetPath);
    void layoutForWindow(sf::Vector2u windowSize);

    void drawBackground(sf::RenderWindow& window) const;
    void drawTitle(sf::RenderWindow& window, const std::string& title, float y) const;
    void drawLines(
        sf::RenderWindow& window,
        const std::vector<std::string>& lines,
        float startY,
        unsigned charSize = 24,
        float startX = 120.f
    ) const;

    // Returns true if Volver was clicked.
    bool handleBackButton(
        sf::RenderWindow& window,
        sf::Vector2f mousePos,
        bool click
    );

    void drawBackButton(sf::RenderWindow& window, bool hovered) const;

protected:
    sf::Sprite background;
    sf::Font font;
    bool fontLoaded = false;
    sf::FloatRect backButtonBounds;
};
