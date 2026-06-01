#include "MainMenu.h"

#include "../../audio/AudioManager.h"
#include "../../render/TextureCache.h"
#include "../../utils/AssetPaths.h"

MainMenu::MainMenu() {
    background.setTexture(
        TextureCache::instance().get(AssetPaths::getMenuStartBackground())
    );
    hoverHighlight.setFillColor(sf::Color(255, 230, 100, 70));
    hoverHighlight.setOutlineColor(sf::Color(255, 240, 160, 140));
    hoverHighlight.setOutlineThickness(2.f);
}

void MainMenu::buildHitAreas(sf::Vector2u windowSize) {
    buttons.clear();

    const float left   = windowSize.x * 0.367f;
    const float width  = windowSize.x * 0.266f;
    const float height = windowSize.y * 0.062f;
    const float step   = windowSize.y * 0.081f;
    float top = windowSize.y * 0.575f;

    auto add = [&](GameState state, bool exitApp) {
        HitButton btn;
        btn.bounds = sf::FloatRect(left, top, width, height);
        btn.targetState = state;
        btn.exitApp = exitApp;
        buttons.push_back(btn);
        top += step;
    };

    add(GameState::NameEntry, false);
    add(GameState::Controls, false);
    add(GameState::Instructions, false);
    add(GameState::Statistics, false);
    add(GameState::MainMenu, true);
    buttons.back().targetState = GameState::MainMenu;
}

void MainMenu::layoutForWindow(sf::Vector2u windowSize) {
    const sf::Texture* tex = background.getTexture();
    if (tex && tex->getSize().x > 0 && tex->getSize().y > 0) {
        background.setScale(
            windowSize.x / static_cast<float>(tex->getSize().x),
            windowSize.y / static_cast<float>(tex->getSize().y)
        );
    }
    background.setPosition(0.f, 0.f);
    buildHitAreas(windowSize);
}

MainMenu::MenuResult MainMenu::handleEvent(
    const sf::Event& event,
    sf::RenderWindow& window
) {
    MenuResult result;
    sf::Vector2f mouse(
        static_cast<float>(event.mouseButton.x),
        static_cast<float>(event.mouseButton.y)
    );

    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {
        for (const auto& btn : buttons) {
            if (btn.bounds.contains(mouse)) {
                if (btn.exitApp) {
                    result.closeApp = true;
                } else {
                    result.nextState = btn.targetState;
                }
                AudioManager::instance().playSound("pickup");
                return result;
            }
        }
    }

    (void)window;
    return result;
}

void MainMenu::update(sf::Vector2f mousePos) {
    hoveredIndex = -1;
    for (size_t i = 0; i < buttons.size(); ++i) {
        if (buttons[i].bounds.contains(mousePos)) {
            hoveredIndex = static_cast<int>(i);
            break;
        }
    }

    if (hoveredIndex != lastHoveredIndex && hoveredIndex >= 0) {
        AudioManager::instance().playSound("pickup");
    }
    lastHoveredIndex = hoveredIndex;

    if (hoveredIndex >= 0) {
        const auto& b = buttons[static_cast<size_t>(hoveredIndex)].bounds;
        hoverHighlight.setPosition(b.left, b.top);
        hoverHighlight.setSize({b.width, b.height});
    }
}

void MainMenu::draw(sf::RenderWindow& window) const {
    window.draw(background);
    if (hoveredIndex >= 0) {
        window.draw(hoverHighlight);
    }
}
