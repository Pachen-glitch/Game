#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <vector>
#include <SFML/System/Clock.hpp>

class Map;

class TilemapRenderer {
public:
    TilemapRenderer();

    void draw(sf::RenderWindow& window, const Map& map);

private:
    sf::Sprite sprite;

    const sf::Texture* floorTexture;
    const sf::Texture* doorTexture;
    const sf::Texture* wallTopTexture;
    const sf::Texture* wallBottomTexture;
    const sf::Texture* wallLeftTexture;
    const sf::Texture* wallRightTexture;
    const sf::Texture* cornerTLTexture;
    const sf::Texture* cornerTRTexture;
    const sf::Texture* cornerBLTexture;
    const sf::Texture* cornerBRTexture;
    const sf::Texture* columnTexture;
    std::vector<const sf::Texture*> fireFrames;
    sf::Clock fireClock;
    const sf::Texture* bushTexture;
    const sf::Texture* rockTexture;
    const sf::Texture* waterTexture;
    const sf::Texture* pitTexture;

    float fireTimer = 0.f;

    
};