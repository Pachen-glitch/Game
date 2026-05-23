#pragma once

#include <SFML/Graphics/Texture.hpp>
#include <string>
#include <unordered_map>
#include <unordered_set>

class TextureCache {
public:
    static TextureCache& instance();

    const sf::Texture& get(const std::string& path);
    bool has(const std::string& path) const;
    bool loadSucceeded(const std::string& path) const;

    void clear();

private:
    TextureCache() = default;
    void ensureFallback();

    std::unordered_map<std::string, sf::Texture> textures;
    std::unordered_set<std::string> failedPaths;
    std::unordered_set<std::string> loggedFailures;
    sf::Texture fallback;
    bool fallbackReady = false;
};
