#include "NameEntryScreen.h"

#include "../../audio/AudioManager.h"
#include "../../save/HighScore.h"
#include "../../utils/AssetPaths.h"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Window/Keyboard.hpp>

NameEntryScreen::NameEntryScreen() {
    setBackgroundPath(AssetPaths::getMenuFondoBackground());
}

void NameEntryScreen::prepare(sf::Vector2u windowSize) {
    layoutForWindow(windowSize);

    const float panelW = 560.f;
    const float panelH = 320.f;
    const float panelX = windowSize.x / 2.f - panelW / 2.f;
    const float panelY = windowSize.y / 2.f - panelH / 2.f;

    panelBounds = {panelX, panelY, panelW, panelH};
    inputBounds = {panelX + 60.f, panelY + 150.f, panelW - 120.f, 46.f};
    confirmBounds = {panelX + panelW / 2.f - 150.f, panelY + 230.f, 300.f, 48.f};
}

void NameEntryScreen::resetInput() {
    inputText.clear();
    enteredName.clear();
}

void NameEntryScreen::confirm() {
    enteredName = normalizePlayerName(inputText);
    inputText = enteredName;
    AudioManager::instance().playSound("pickup");
}

GameState NameEntryScreen::handleEvent(
    const sf::Event& event,
    sf::RenderWindow& window
) {
    if (event.type == sf::Event::MouseMoved) {
        update(sf::Vector2f(
            static_cast<float>(event.mouseMove.x),
            static_cast<float>(event.mouseMove.y)
        ));
    }

    if (event.type == sf::Event::TextEntered) {
        if (event.text.unicode == 8 || event.text.unicode == 127) {
            if (!inputText.empty()) {
                inputText.pop_back();
            }
            return GameState::NameEntry;
        }

        if (event.text.unicode >= 32 && event.text.unicode < 127) {
            if (inputText.size() < static_cast<size_t>(kMaxPlayerNameLength)) {
                inputText.push_back(static_cast<char>(event.text.unicode));
            }
        }
        return GameState::NameEntry;
    }

    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Enter ||
            event.key.code == sf::Keyboard::Return) {
            confirm();
            return GameState::Playing;
        }
        if (event.key.code == sf::Keyboard::Escape) {
            resetInput();
            return GameState::MainMenu;
        }
    }

    if (event.type == sf::Event::MouseButtonPressed &&
        event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mouse(
            static_cast<float>(event.mouseButton.x),
            static_cast<float>(event.mouseButton.y)
        );

        if (confirmBounds.contains(mouse)) {
            confirm();
            return GameState::Playing;
        }
    }

    (void)window;
    return GameState::NameEntry;
}

void NameEntryScreen::update(sf::Vector2f mousePos) {
    confirmHovered = confirmBounds.contains(mousePos);
}

void NameEntryScreen::draw(sf::RenderWindow& window) const {
    drawBackground(window);

    sf::RectangleShape dim;
    dim.setSize(sf::Vector2f(window.getSize()));
    dim.setFillColor(sf::Color(0, 0, 0, 140));
    window.draw(dim);

    if (!fontLoaded) return;

    sf::RectangleShape panel;
    panel.setPosition(panelBounds.left, panelBounds.top);
    panel.setSize({panelBounds.width, panelBounds.height});
    panel.setFillColor(sf::Color(35, 22, 12, 230));
    panel.setOutlineColor(sf::Color(255, 210, 90));
    panel.setOutlineThickness(3.f);
    window.draw(panel);

    sf::Text prompt;
    prompt.setFont(font);
    prompt.setString("\xC2\xBF" "Cu\xC3" "\xA1" "l es tu nombre, aventurero?");
    prompt.setCharacterSize(28);
    prompt.setFillColor(sf::Color(255, 230, 150));
    prompt.setOutlineColor(sf::Color(30, 18, 8));
    prompt.setOutlineThickness(1.f);
    {
        sf::FloatRect b = prompt.getLocalBounds();
        prompt.setOrigin(b.width / 2.f, b.height / 2.f);
        prompt.setPosition(
            panelBounds.left + panelBounds.width / 2.f,
            panelBounds.top + 70.f
        );
    }
    window.draw(prompt);

    sf::RectangleShape field;
    field.setPosition(inputBounds.left, inputBounds.top);
    field.setSize({inputBounds.width, inputBounds.height});
    field.setFillColor(sf::Color(18, 12, 8, 255));
    field.setOutlineColor(sf::Color(255, 220, 120));
    field.setOutlineThickness(2.f);
    window.draw(field);

    std::string display = inputText.empty()
        ? std::string("_")
        : inputText + "_";

    sf::Text input;
    input.setFont(font);
    input.setString(display);
    input.setCharacterSize(26);
    input.setFillColor(sf::Color::White);
    input.setPosition(inputBounds.left + 12.f, inputBounds.top + 8.f);
    window.draw(input);

    sf::RectangleShape button;
    button.setPosition(confirmBounds.left, confirmBounds.top);
    button.setSize({confirmBounds.width, confirmBounds.height});
    button.setFillColor(confirmHovered
        ? sf::Color(255, 210, 70, 220)
        : sf::Color(90, 60, 25, 220));
    button.setOutlineColor(sf::Color(255, 230, 140));
    button.setOutlineThickness(2.f);
    window.draw(button);

    sf::Text label;
    label.setFont(font);
    label.setString("Comenzar aventura");
    label.setCharacterSize(24);
    label.setFillColor(sf::Color(30, 20, 10));
    {
        sf::FloatRect b = label.getLocalBounds();
        label.setOrigin(b.width / 2.f, b.height / 2.f);
        label.setPosition(
            confirmBounds.left + confirmBounds.width / 2.f,
            confirmBounds.top + confirmBounds.height / 2.f
        );
    }
    window.draw(label);
}
