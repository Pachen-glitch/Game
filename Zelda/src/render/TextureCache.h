#pragma once

#include <SFML/Graphics/Texture.hpp>
#include <string>
#include <unordered_map>

// Singleton texture cache — load each path once per run.
class TextureCache {
public:
    static TextureCache& instance();

    const sf::Texture& get(const std::string& path);
    bool has(const std::string& path) const;

    void clear();

private:
    TextureCache() = default;
    std::unordered_map<std::string, sf::Texture> textures;
    sf::Texture fallback;
    bool fallbackReady = false;

    void ensureFallback();
};
