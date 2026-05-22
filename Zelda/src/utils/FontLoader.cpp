#include "FontLoader.h"

#ifdef _WIN32
#include <windows.h>
#endif

#include <filesystem>

std::vector<std::string> FontLoader::defaultPaths() {
    std::vector<std::string> paths = {
        "assets/fonts/arial.ttf",
        "assets/fonts/default.ttf",
    };

#ifdef _WIN32
    char winDir[MAX_PATH];
    if (GetEnvironmentVariableA("WINDIR", winDir, MAX_PATH) > 0) {
        std::string base(winDir);
        paths.push_back(base + "/Fonts/arial.ttf");
        paths.push_back(base + "/Fonts/segoeui.ttf");
        paths.push_back(base + "/Fonts/consola.ttf");
        paths.push_back(base + "/Fonts/tahoma.ttf");
    }
#endif

    return paths;
}

bool FontLoader::load(sf::Font& font, const std::vector<std::string>& extraPaths) {
    auto paths = defaultPaths();
    paths.insert(paths.end(), extraPaths.begin(), extraPaths.end());

    for (const auto& path : paths) {
        if (!std::filesystem::exists(path)) continue;
        if (font.loadFromFile(path)) return true;
    }
    return false;
}
