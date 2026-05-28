#include "AssetPaths.h"

#include "../entity/enemy/EnemyState.h"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <iostream>
#include <unordered_map>
#include <unordered_set>

#ifdef _WIN32
#include <windows.h>
#endif

namespace fs = std::filesystem;

namespace {

std::string gAssetsRoot;
std::string gSpritesRoot;
std::unordered_set<std::string> gKnownRelative;
std::unordered_map<std::string, std::string> gRelToAbs;
std::unordered_set<std::string> gMissingLogged;
std::unordered_map<std::string, std::string> gResolveCache;

std::string toLower(std::string s) {
    for (char& c : s) {
        if (c >= 'A' && c <= 'Z') c = static_cast<char>(c - 'A' + 'a');
    }
    return s;
}

std::string normalizeRel(std::string rel) {
    for (char& c : rel) {
        if (c == '\\') c = '/';
    }
    if (!rel.empty() && rel.front() == '/') rel.erase(rel.begin());
    return toLower(rel);
}

fs::path getExecutableDirectory() {
#ifdef _WIN32
    wchar_t buffer[MAX_PATH];
    DWORD len = GetModuleFileNameW(nullptr, buffer, MAX_PATH);
    if (len > 0 && len < MAX_PATH) {
        return fs::path(buffer).parent_path();
    }
#endif
    return fs::current_path();
}

fs::path findAssetsRoot(const fs::path& exeDir) {
    std::vector<fs::path> candidates = {
        exeDir / "assets",
        exeDir / ".." / "assets",
        exeDir / ".." / ".." / "assets",
        exeDir / ".." / ".." / ".." / "assets",
    };

    for (const auto& c : candidates) {
        std::error_code ec;
        auto canonical = fs::weakly_canonical(c, ec);
        if (ec) continue;
        if (fs::exists(canonical / "sprites")) {
            return canonical;
        }
    }
    return exeDir / "assets";
}

} // namespace

void AssetPaths::initialize() {
    gKnownRelative.clear();
    gRelToAbs.clear();
    gMissingLogged.clear();
    gResolveCache.clear();

    fs::path exeDir = getExecutableDirectory();
    std::error_code ec;
    fs::current_path(exeDir, ec);

    fs::path root = findAssetsRoot(exeDir);
    gAssetsRoot = root.string();
    for (char& c : gAssetsRoot) {
        if (c == '\\') c = '/';
    }

    gSpritesRoot = gAssetsRoot + "/sprites";
    scanSpriteTree();

    std::cout << "[AssetPaths] assets root: " << gAssetsRoot << "\n";
    std::cout << "[AssetPaths] sprites indexed: " << gKnownRelative.size() << " PNG(s)\n";
}

const std::string& AssetPaths::assetsRoot() { return gAssetsRoot; }
const std::string& AssetPaths::spritesRoot() { return gSpritesRoot; }

void AssetPaths::scanSpriteTree() {
    if (!fs::exists(gSpritesRoot)) return;

    for (const auto& entry : fs::recursive_directory_iterator(gSpritesRoot)) {
        if (!entry.is_regular_file()) continue;
        auto ext = entry.path().extension().string();
        if (ext != ".png" && ext != ".PNG") continue;

        std::error_code ec;
        auto rel = fs::relative(entry.path(), gSpritesRoot, ec);
        if (ec) continue;
        std::string norm = normalizeRel(rel.string());
        gKnownRelative.insert(norm);
        gRelToAbs[norm] = entry.path().string();
    }
}

int AssetPaths::parseFrameIndex(const std::string& filename, const std::string& prefix) {
    std::string lower = toLower(filename);
    std::string pref = toLower(prefix);

    if (lower.rfind(pref, 0) != 0) return -1;

    std::string rest = lower.substr(pref.size());
    const auto dot = rest.find(".png");
    if (dot == std::string::npos) return -1;

    rest = rest.substr(0, dot);
    if (rest.empty()) return -1;

    int index = 0;
    for (char c : rest) {
        if (!std::isdigit(static_cast<unsigned char>(c))) return -1;
        index = index * 10 + (c - '0');
    }
    return index;
}

std::vector<std::string> AssetPaths::collectFramesByPrefix(
    const std::string& spriteFolder,
    const std::string& prefix
) {
    std::string folder = normalizeRel(spriteFolder);
    if (!folder.empty() && folder.back() != '/') folder += '/';

    std::string pref = toLower(prefix);
    std::vector<std::pair<int, std::string>> matches;

    for (const auto& [rel, abs] : gRelToAbs) {
        if (rel.rfind(folder, 0) != 0) continue;

        std::string filename = rel.substr(folder.size());
        int frameIndex = parseFrameIndex(filename, pref);
        if (frameIndex < 0) continue;

        matches.emplace_back(frameIndex, abs);
    }

    std::sort(matches.begin(), matches.end(),
        [](const auto& a, const auto& b) { return a.first < b.first; });

    std::vector<std::string> paths;
    paths.reserve(matches.size());
    for (const auto& m : matches) {
        paths.push_back(m.second);
    }
    return paths;
}

Animation AssetPaths::buildAnimation(
    const std::vector<std::string>& framePaths,
    float frameDuration,
    bool loop
) {
    Animation anim;
    for (const auto& path : framePaths) {
        if (!path.empty()) {
            anim.addFrame(path, frameDuration);
        }
    }
    anim.setLooping(loop);
    return anim;
}

bool AssetPaths::exists(const std::string& path) {
    if (path.empty()) return false;
    if (fs::exists(path)) return true;

    std::string norm = normalizeRel(path);
    if (norm.rfind("sprites/", 0) == 0) norm = norm.substr(8);
    return gKnownRelative.count(norm) > 0;
}

std::string AssetPaths::absolutePath(const std::string& relativeFromAssets) {
    std::string rel = relativeFromAssets;
    for (char& c : rel) {
        if (c == '\\') c = '/';
    }
    if (!rel.empty() && rel[0] == '/') rel.erase(0, 1);
    if (rel.rfind("assets/", 0) == 0) {
        return gAssetsRoot + "/" + rel.substr(7);
    }
    if (rel.rfind("sprites/", 0) == 0) {
        return gAssetsRoot + "/" + rel;
    }
    return gAssetsRoot + "/" + rel;
}

void AssetPaths::logMissingOnce(const std::string& logicalId, const std::string& detail) {
    if (gMissingLogged.count(logicalId)) return;
    gMissingLogged.insert(logicalId);
    std::cerr << "[AssetPaths] MISSING " << logicalId;
    if (!detail.empty()) std::cerr << " (" << detail << ")";
    std::cerr << "\n";
}

std::vector<std::string> AssetPaths::getPlayerIdleFrames(Direction direction) {
    char d = directionSuffix(direction);
    std::string pref = std::string("idle_") + d;
    auto frames = collectFramesByPrefix("player/idle", pref);
    if (!frames.empty()) return frames;

    logMissingOnce("player_idle_" + std::string(1, d), "player/idle/" + pref);
    return {};
}

std::vector<std::string> AssetPaths::getPlayerWalkFrames(Direction direction) {
    char d = directionSuffix(direction);
    std::string pref = std::string("walk_") + d;
    auto frames = collectFramesByPrefix("player/move", pref);
    if (!frames.empty()) return frames;

    logMissingOnce("player_walk_" + std::string(1, d), "player/move/" + pref);
    return {};
}

std::vector<std::string> AssetPaths::getPlayerAttackFrames(Direction direction) {
    char d = directionSuffix(direction);
    std::string prefDir = std::string("attack_") + d;
    auto frames = collectFramesByPrefix("player/attack", prefDir);
    if (!frames.empty()) return frames;

    return collectFramesByPrefix("player/attack", "attack_");
}

std::vector<std::string> AssetPaths::getEnemyAnimFrames(
    const std::string& type,
    const std::string& state
) {
    std::string folder = "enemies/" + type;
    std::string pref = state + "_";
    auto frames = collectFramesByPrefix(folder, pref);

    if (!frames.empty()) return frames;

    logMissingOnce("enemy_" + type + "_" + state, folder + "/" + pref);
    return {};
}

char AssetPaths::directionSuffix(Direction direction) {
    switch (direction) {
        case Direction::DOWN: return 'd';
        case Direction::UP: return 'u';
        case Direction::LEFT: return 'l';
        case Direction::RIGHT: return 'r';
    }
    return 'd';
}

std::string AssetPaths::getPlayerWalkSprite(Direction direction, int frame) {
    auto frames = getPlayerWalkFrames(direction);
    if (frames.empty()) {
        return resolveFirst({"sprites/player/move/walk_d1.png"});
    }
    int idx = std::max(0, std::min(frame - 1, static_cast<int>(frames.size()) - 1));
    return frames[idx];
}

std::string AssetPaths::getPlayerIdleSprite(Direction direction) {
    auto frames = getPlayerIdleFrames(direction);
    if (!frames.empty()) return frames[0];
    return getPlayerWalkSprite(direction, 1);
}

std::string AssetPaths::findPngInFolder(
    const std::string& spriteFolder,
    const std::vector<std::string>& preferredNames
) {
    std::string folder = normalizeRel(spriteFolder);
    if (!folder.empty() && folder.back() != '/') folder += '/';

    for (const auto& name : preferredNames) {
        std::string candidate = folder + normalizeRel(name);
        auto it = gRelToAbs.find(candidate);
        if (it != gRelToAbs.end()) return it->second;

        fs::path direct = fs::path(gSpritesRoot) / spriteFolder / name;
        if (fs::exists(direct)) return direct.string();
    }

    for (const auto& [rel, abs] : gRelToAbs) {
        if (rel.rfind(folder, 0) == 0) return abs;
    }
    return "";
}

std::string AssetPaths::resolveFirst(const std::vector<std::string>& relativeCandidates) {
    std::string cacheKey;
    for (const auto& c : relativeCandidates) cacheKey += c + '|';

    auto cached = gResolveCache.find(cacheKey);
    if (cached != gResolveCache.end()) return cached->second;

    for (const auto& rel : relativeCandidates) {
        std::string norm = normalizeRel(rel);
        if (norm.rfind("sprites/", 0) == 0) norm = norm.substr(8);

        auto indexed = gRelToAbs.find(norm);
        if (indexed != gRelToAbs.end()) {
            gResolveCache[cacheKey] = indexed->second;
            return indexed->second;
        }

        std::string abs = absolutePath(rel);
        if (fs::exists(abs)) {
            gResolveCache[cacheKey] = abs;
            return abs;
        }
    }

    if (!relativeCandidates.empty()) {
        logMissingOnce(cacheKey, relativeCandidates.front());
        gResolveCache[cacheKey] = absolutePath(relativeCandidates.front());
        return gResolveCache[cacheKey];
    }
    return "";
}

std::string AssetPaths::getEnemySprite(const std::string& type, const std::string& state, int frame) {
    auto frames = getEnemyAnimFrames(type, state);
    if (!frames.empty()) {
        int idx = std::max(0, std::min(frame - 1, static_cast<int>(frames.size()) - 1));
        return frames[idx];
    }
    return findPngInFolder("enemies/" + type, {type + ".png", "idle.png"});
}

std::string AssetPaths::getEnemySprite(EnemyKind kind, const std::string& state, int frame) {
    switch (kind) {
        case EnemyKind::Slime: return getEnemySprite("slime", state, frame);
        case EnemyKind::Bat: return getEnemySprite("bat", state, frame);
        case EnemyKind::Skeleton: return getEnemySprite("skeleton", state, frame);
        case EnemyKind::Summoner: return getEnemySprite("summoner", state, frame);
    }
    return getEnemySprite("slime", state, frame);
}

std::string AssetPaths::getHeartSprite() {
    std::string found = findPngInFolder("objects/hearts", {"heart.png", "heath.png"});
    if (!found.empty()) return found;
    return resolveFirst({"sprites/objects/hearts/heart.png"});
}

std::string AssetPaths::getKeySprite() {
    std::string found = findPngInFolder("objects/keys", {"key.png"});
    if (!found.empty()) return found;
    return resolveFirst({"sprites/objects/keys/key.png"});
}

std::string AssetPaths::getCoinSprite() {
    std::string found = findPngInFolder("objects/coins", {"coin.png", "rupee.png"});
    if (!found.empty()) return found;
    return resolveFirst({"sprites/objects/coins/coin.png"});
}

std::string AssetPaths::getDoorSprite() {
    std::string found = findPngInFolder("objects/door", {"door.png"});
    if (!found.empty()) return found;
    found = findPngInFolder("map/doors", {"door.png"});
    if (!found.empty()) return found;
    return resolveFirst({"sprites/objects/door/door.png"});
}

std::string AssetPaths::getShopkeeperSprite() {
    std::string found = findPngInFolder("ui/menu", {"shopkeeper.png"});
    if (!found.empty()) return found;
    return resolveFirst({"sprites/npc/shopkeeper.png"});
}

std::string AssetPaths::getFontPath() {
    return resolveFirst({"fonts/arial.ttf", "fonts/default.ttf"});
}

std::string AssetPaths::getAudioPath(const std::string& filename) {
    return resolveFirst({"audio/" + filename});
}

void AssetPaths::validateCriticalAssets() {
    std::cout << "[AssetPaths] Validating animations...\n";

    auto logClip = [](const std::string& name, const std::vector<std::string>& frames) {
        if (frames.empty()) {
            std::cout << "  [X] " << name << " (0 frames)\n";
        } else {
            std::cout << "  [ok] " << name << " (" << frames.size() << " frames)\n";
        }
    };

    logClip("player_idle_down", getPlayerIdleFrames(Direction::DOWN));
    logClip("player_walk_down", getPlayerWalkFrames(Direction::DOWN));
    logClip("slime_idle", getEnemyAnimFrames("slime", "idle"));
    logClip("slime_walk", getEnemyAnimFrames("slime", "walk"));
    logClip("slime_hurt", getEnemyAnimFrames("slime", "hurt"));
    logClip("slime_death", getEnemyAnimFrames("slime", "death"));
}

std::string AssetPaths::getChestClosedSprite() {

    std::string found = findPngInFolder(
        "objects/chess",
        {"Chess_close.png"}
    );

    if (!found.empty())
        return found;

    return resolveFirst({
        "sprites/objects/chess/Chess_close.png"
    });
}

std::string AssetPaths::getChestOpenSprite() {

    std::string found = findPngInFolder(
        "objects/chess",
        {"Chess_open.png"}
    );

    if (!found.empty())
        return found;

    return resolveFirst({
        "sprites/objects/chess/Chess_open.png"
    });
}

std::string AssetPaths::getRandomRupeeSprite() {

    static std::vector<std::string> rupees = {

        resolveFirst({
            "sprites/objects/coins/gema_azul.png"
        }),

        resolveFirst({
            "sprites/objects/coins/gema_diamond.png"
        }),

        resolveFirst({
            "sprites/objects/coins/gema_roja.png"
        })
    };

    return rupees[rand() % rupees.size()];
}