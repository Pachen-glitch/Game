#include "TextureCache.h"

#include <SFML/Graphics/Image.hpp>
#include <iostream>

TextureCache& TextureCache::instance() {
    static TextureCache cache;
    return cache;
}

void TextureCache::ensureFallback() {
    if (fallbackReady) return;
    sf::Image img;
    img.create(48, 48, sf::Color(200, 80, 200));
    fallback.loadFromImage(img);
    fallbackReady = true;
}

const sf::Texture& TextureCache::get(const std::string& path) {
    if (path.empty()) {
        ensureFallback();
        return fallback;
    }

    auto it = textures.find(path);
    if (it != textures.end()) {
        return it->second;
    }

    sf::Texture tex;
    if (!tex.loadFromFile(path)) {
        std::cerr << "TextureCache: failed to load " << path << std::endl;
        ensureFallback();
        return fallback;
    }

    textures[path] = std::move(tex);
    return textures[path];
}

bool TextureCache::has(const std::string& path) const {
    return textures.find(path) != textures.end();
}

void TextureCache::clear() {
    textures.clear();
}
