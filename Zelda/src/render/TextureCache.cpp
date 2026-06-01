#include "TextureCache.h"

#include <SFML/Graphics/Image.hpp>
#include <filesystem>
#include <iostream>

TextureCache& TextureCache::instance() {
    static TextureCache cache;
    return cache;
}
// Asegura el fallback
void TextureCache::ensureFallback() {
    if (fallbackReady) return;
    sf::Image img;
    img.create(48, 48, sf::Color(200, 80, 200));
    fallback.loadFromImage(img);
    fallbackReady = true;
}
// Devuelve el texture para el path
const sf::Texture& TextureCache::get(const std::string& path) {
    ensureFallback();

    if (path.empty()) {
        return fallback;
    }

    if (failedPaths.count(path)) {
        return fallback;
    }

    auto it = textures.find(path);
    if (it != textures.end()) {
        return it->second;
    }
// Si el archivo no existe, se registra como fallido
    if (!std::filesystem::exists(path)) {
        if (!loggedFailures.count(path)) {
            loggedFailures.insert(path);
            std::cerr << "[TextureCache] file not found: " << path << "\n";
        }
        failedPaths.insert(path);
        return fallback;
    }

    sf::Texture tex;
    if (!tex.loadFromFile(path)) {
        if (!loggedFailures.count(path)) {
            loggedFailures.insert(path);
            std::cerr << "[TextureCache] failed to load: " << path << "\n";
        }
        failedPaths.insert(path);
        return fallback;
    }

    textures[path] = std::move(tex);
    return textures[path];
}
// Devuelve si el texture existe para el path
bool TextureCache::has(const std::string& path) const {
    return textures.find(path) != textures.end();
}
// Devuelve si el texture se cargo correctamente para el path
bool TextureCache::loadSucceeded(const std::string& path) const {
    return textures.find(path) != textures.end();
}

void TextureCache::clear() {
    textures.clear();
    failedPaths.clear();
    loggedFailures.clear();
}
