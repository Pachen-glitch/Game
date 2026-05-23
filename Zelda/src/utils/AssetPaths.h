#pragma once

#include "../animation/Animation.h"
#include "../entity/player/Direction.h"

#include <string>
#include <vector>

enum class EnemyKind;

class AssetPaths {
public:
    static void initialize();
    static void validateCriticalAssets();

    static const std::string& assetsRoot();
    static const std::string& spritesRoot();

    // Dynamic frame discovery — sorted numerically, no hardcoded counts.
    static std::vector<std::string> collectFramesByPrefix(
        const std::string& spriteFolder,
        const std::string& prefix
    );

    static Animation buildAnimation(
        const std::vector<std::string>& framePaths,
        float frameDuration,
        bool loop
    );

    static std::vector<std::string> getPlayerIdleFrames(Direction direction);
    static std::vector<std::string> getPlayerWalkFrames(Direction direction);
    static std::vector<std::string> getPlayerAttackFrames(Direction direction);

    static std::vector<std::string> getEnemyAnimFrames(
        const std::string& type,
        const std::string& state
    );

    // Single-frame fallbacks (first frame of clip or resolve).
    static std::string getPlayerWalkSprite(Direction direction, int frame);
    static std::string getPlayerIdleSprite(Direction direction);
    static std::string getEnemySprite(const std::string& type, const std::string& state = "idle", int frame = 1);
    static std::string getEnemySprite(EnemyKind kind, const std::string& state = "idle", int frame = 1);

    static std::string getHeartSprite();
    static std::string getKeySprite();
    static std::string getCoinSprite();
    static std::string getDoorSprite();
    static std::string getShopkeeperSprite();

    static std::string getFontPath();
    static std::string getAudioPath(const std::string& filename);

    static bool exists(const std::string& path);
    static std::string resolveFirst(const std::vector<std::string>& relativeCandidates);

private:
    static char directionSuffix(Direction direction);
    static std::string findPngInFolder(
        const std::string& spriteFolder,
        const std::vector<std::string>& preferredNames
    );
    static std::string absolutePath(const std::string& relativeFromAssets);
    static void scanSpriteTree();
    static void logMissingOnce(const std::string& logicalId, const std::string& detail);
    static int parseFrameIndex(const std::string& filename, const std::string& prefix);
};
