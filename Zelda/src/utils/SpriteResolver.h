#pragma once

#include <initializer_list>
#include <string>
#include <vector>

// Prueba varias rutas y usa la primera que exista en disco.
class SpriteResolver {
public:
    static std::string resolve(std::initializer_list<const char*> candidates);
    static std::vector<std::string> scanDirectory(const std::string& folder);
    static void logAvailableSprites();
};
