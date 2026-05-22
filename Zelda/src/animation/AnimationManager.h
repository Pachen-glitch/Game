#pragma once

#include "Animation.h"

#include <SFML/Graphics/Sprite.hpp>
#include <string>
#include <unordered_map>

// Owns named clips and drives a sprite from the texture cache.
class AnimationManager {
public:
    void registerAnimation(const std::string& name, Animation anim);
    bool play(const std::string& name, bool restart = true);
    void update(float dt);

    void applyToSprite(sf::Sprite& sprite, float scale = 2.f);
    const std::string& currentClip() const { return activeName; }
    bool isFinished() const;

private:
    std::unordered_map<std::string, Animation> clips;
    std::string activeName;
    bool hasActive = false;
};
