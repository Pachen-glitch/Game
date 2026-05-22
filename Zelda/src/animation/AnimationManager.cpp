#include "AnimationManager.h"
#include "../render/TextureCache.h"

void AnimationManager::registerAnimation(const std::string& name, Animation anim) {
    clips[name] = std::move(anim);
}

bool AnimationManager::play(const std::string& name, bool restart) {
    auto it = clips.find(name);
    if (it == clips.end()) return false;

    if (restart || activeName != name) {
        it->second.reset();
    }

    activeName = name;
    hasActive = true;
    return true;
}

void AnimationManager::update(float dt) {
    if (!hasActive) return;
    auto it = clips.find(activeName);
    if (it == clips.end()) return;
    it->second.update(dt);
}

bool AnimationManager::isFinished() const {
    if (!hasActive) return true;
    auto it = clips.find(activeName);
    if (it == clips.end()) return true;
    return it->second.isFinished();
}

void AnimationManager::applyToSprite(sf::Sprite& sprite, float scale) {
    if (!hasActive) return;
    auto it = clips.find(activeName);
    if (it == clips.end()) return;

    const std::string& path = it->second.getCurrentFramePath();
    const sf::Texture& tex = TextureCache::instance().get(path);
    sprite.setTexture(tex, true);
    sprite.setScale(scale, scale);
}
