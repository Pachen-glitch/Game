#pragma once

#include <SFML/Graphics/Font.hpp>
#include <string>
#include <vector>

// Carga una fuente una sola vez — evita spam en consola si falta el archivo.
class FontLoader {
public:
    static bool load(sf::Font& font, const std::vector<std::string>& extraPaths = {});

private:
    static std::vector<std::string> defaultPaths();
};
