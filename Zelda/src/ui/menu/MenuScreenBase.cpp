#include "MenuScreenBase.h"

#include "../../render/TextureCache.h"
#include "../../utils/FontLoader.h"

#include <SFML/Graphics/RectangleShape.hpp>

MenuScreenBase::MenuScreenBase() {
    fontLoaded = FontLoader::load(font);
}

void MenuScreenBase::setBackgroundPath(const std::string& assetPath) {
    background.setTexture(TextureCache::instance().get(assetPath));
    background.setPosition(0.f, 0.f);
}

void MenuScreenBase::layoutForWindow(sf::Vector2u windowSize) {
    const sf::Texture* tex = background.getTexture();
    if (!tex) return;

    sf::Vector2u texSize = tex->getSize();
    if (texSize.x == 0 || texSize.y == 0) return;

    background.setScale(
        windowSize.x / static_cast<float>(texSize.x),
        windowSize.y / static_cast<float>(texSize.y)
    );

    float btnW = 220.f;
    float btnH = 44.f;
    backButtonBounds = sf::FloatRect(
        windowSize.x / 2.f - btnW / 2.f,
        windowSize.y - 90.f,
        btnW,
        btnH
    );
}

void MenuScreenBase::drawBackground(sf::RenderWindow& window) const {
    window.draw(background);
}

void MenuScreenBase::drawTitle(
    sf::RenderWindow& window,
    const std::string& title,
    float y
) const {
    if (!fontLoaded) return;

    sf::Text text;
    text.setFont(font);
    text.setString(title);
    text.setCharacterSize(42);
    text.setFillColor(sf::Color(255, 230, 140));
    text.setOutlineColor(sf::Color(40, 25, 10));
    text.setOutlineThickness(2.f);

    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
    text.setPosition(window.getSize().x / 2.f, y);
    window.draw(text);
}

void MenuScreenBase::drawLines(
    sf::RenderWindow& window,
    const std::vector<std::string>& lines,
    float startY,
    unsigned charSize
) const {
    if (!fontLoaded) return;

    float y = startY;
    for (const auto& line : lines) {
        sf::Text text;
        text.setFont(font);
        text.setString(line);
        text.setCharacterSize(charSize);
        text.setFillColor(sf::Color(255, 248, 220));
        text.setOutlineColor(sf::Color(20, 20, 30));
        text.setOutlineThickness(1.f);
        text.setPosition(120.f, y);
        window.draw(text);
        y += static_cast<float>(charSize) + 14.f;
    }
}

void MenuScreenBase::drawBackButton(sf::RenderWindow& window, bool hovered) const {
    if (!fontLoaded) return;

    sf::RectangleShape box;
    box.setPosition(backButtonBounds.left, backButtonBounds.top);
    box.setSize({backButtonBounds.width, backButtonBounds.height});
    box.setFillColor(hovered
        ? sf::Color(255, 220, 80, 180)
        : sf::Color(60, 45, 30, 160));
    box.setOutlineColor(sf::Color(255, 230, 140));
    box.setOutlineThickness(2.f);
    window.draw(box);

    sf::Text text;
    text.setFont(font);
    text.setString("Volver");
    text.setCharacterSize(26);
    text.setFillColor(sf::Color::White);
    sf::FloatRect b = text.getLocalBounds();
    text.setOrigin(b.width / 2.f, b.height / 2.f);
    text.setPosition(
        backButtonBounds.left + backButtonBounds.width / 2.f,
        backButtonBounds.top + backButtonBounds.height / 2.f
    );
    window.draw(text);
}

bool MenuScreenBase::handleBackButton(
    sf::RenderWindow& window,
    sf::Vector2f mousePos,
    bool click
) {
    (void)window;
    bool hovered = backButtonBounds.contains(mousePos);
    if (click && hovered) return true;
    return false;
}
