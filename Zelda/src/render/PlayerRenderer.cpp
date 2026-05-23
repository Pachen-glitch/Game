#include "PlayerRenderer.h"
#include "../entity/player/PlayerState.h"
#include "../utils/AssetPaths.h"

PlayerRenderer::PlayerRenderer() {
    buildAnimations();
    anim.play("idle_down");
    anim.applyToSprite(sprite, 2.f);
}

void PlayerRenderer::buildAnimations() {
    registerWalkClips();
    registerIdleClips();
    registerAttackClips();
}

void PlayerRenderer::registerWalkClips() {
    const Direction dirs[] = {Direction::DOWN, Direction::UP, Direction::LEFT, Direction::RIGHT};
    const char* names[] = {"down", "up", "left", "right"};

    for (int d = 0; d < 4; ++d) {
        auto frames = AssetPaths::getPlayerWalkFrames(dirs[d]);
        float duration = frames.size() > 6 ? 0.09f : 0.12f;

        Animation walk = AssetPaths::buildAnimation(frames, duration, true);
        if (walk.getCurrentFramePath().empty()) {
            std::string fallback = AssetPaths::getPlayerWalkSprite(dirs[d], 1);
            walk.addFrame(fallback, 0.15f);
        }
        anim.registerAnimation(std::string("walk_") + names[d], walk);
    }
}

void PlayerRenderer::registerIdleClips() {
    const Direction dirs[] = {Direction::DOWN, Direction::UP, Direction::LEFT, Direction::RIGHT};
    const char* names[] = {"down", "up", "left", "right"};

    for (int d = 0; d < 4; ++d) {
        auto frames = AssetPaths::getPlayerIdleFrames(dirs[d]);
        Animation idle = AssetPaths::buildAnimation(frames, 0.42f, true);

        if (idle.getCurrentFramePath().empty()) {
            idle.addFrame(AssetPaths::getPlayerWalkSprite(dirs[d], 1), 0.4f);
        }
        anim.registerAnimation(std::string("idle_") + names[d], idle);
    }
}

void PlayerRenderer::registerAttackClips() {
    const Direction dirs[] = {Direction::DOWN, Direction::UP, Direction::LEFT, Direction::RIGHT};
    const char* names[] = {"down", "up", "left", "right"};

    for (int d = 0; d < 4; ++d) {
        auto frames = AssetPaths::getPlayerAttackFrames(dirs[d]);
        Animation attack = AssetPaths::buildAnimation(frames, 0.08f, false);
        if (attack.getCurrentFramePath().empty()) {
            attack.addFrame(AssetPaths::getPlayerIdleSprite(dirs[d]), 0.1f);
        }
        anim.registerAnimation(std::string("attack_") + names[d], attack);
    }
}

std::string PlayerRenderer::clipForState(Player& player) const {
    std::string dir;
    switch (player.getFacingDirection()) {
        case Direction::UP: dir = "up"; break;
        case Direction::DOWN: dir = "down"; break;
        case Direction::LEFT: dir = "left"; break;
        case Direction::RIGHT: dir = "right"; break;
    }

    if (player.getState() == PlayerState::Dead) {
        return "idle_down";
    }
    if (player.getState() == PlayerState::Hurt) {
        return "idle_" + dir;
    }
    if (player.getState() == PlayerState::Attack) {
        return "attack_" + dir;
    }
    if (player.getState() == PlayerState::Spin) {
        return "walk_" + dir;
    }
    if (player.getState() == PlayerState::Walk || player.isMoving()) {
        return "walk_" + dir;
    }
    return "idle_" + dir;
}

void PlayerRenderer::update(Player& player, float deltaTime) {
    std::string clip = clipForState(player);
    bool restart = (clip != lastClip);

    if (player.getState() == PlayerState::Attack && clip == lastClip &&
        anim.isFinished()) {
        restart = true;
    }

    if (restart) {
        anim.play(clip, player.getState() == PlayerState::Attack);
        lastClip = clip;
    }

    anim.update(deltaTime);
    anim.applyToSprite(sprite, 2.f);

    if (player.getState() == PlayerState::Hurt) {
        sprite.setColor(sf::Color(255, 120, 120));
    } else {
        sprite.setColor(sf::Color::White);
    }

    sprite.setPosition(player.getPosition());
    player.getSprite() = sprite;
}

void PlayerRenderer::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}
