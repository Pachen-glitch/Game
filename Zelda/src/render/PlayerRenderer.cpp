#include "PlayerRenderer.h"
#include "../entity/player/PlayerState.h"

PlayerRenderer::PlayerRenderer() {
    buildAnimations();
    anim.play("idle_down");
    anim.applyToSprite(sprite, 2.f);
}

void PlayerRenderer::buildAnimations() {
    registerWalkClips();

    Animation idle;
    idle.addFrame("assets/sprites/player/move/walk_d1.png", 0.4f);
    idle.setLooping(true);
    anim.registerAnimation("idle_down", idle);
}

void PlayerRenderer::registerWalkClips() {
    const char* dirs[] = {"d", "u", "l", "r"};
    const char* names[] = {"down", "up", "left", "right"};

    for (int d = 0; d < 4; ++d) {
        Animation walk;
        for (int f = 1; f <= 5; ++f) {
            std::string path = "assets/sprites/player/move/walk_" +
                std::string(dirs[d]) + std::to_string(f) + ".png";
            walk.addFrame(path, 0.12f);
        }
        walk.setLooping(true);
        anim.registerAnimation(std::string("walk_") + names[d], walk);
    }
}

std::string PlayerRenderer::clipForState(Player& player) const {
    std::string dir;
    switch (player.getDirection()) {
        case Direction::UP: dir = "up"; break;
        case Direction::DOWN: dir = "down"; break;
        case Direction::LEFT: dir = "left"; break;
        case Direction::RIGHT: dir = "right"; break;
    }

    switch (player.getState()) {
        case PlayerState::Walk:
            return "walk_" + dir;
        case PlayerState::Attack:
            return "walk_" + dir;
        case PlayerState::Spin:
            return "walk_" + dir;
        case PlayerState::Shield:
            return "idle_" + dir;
        case PlayerState::Hurt:
        case PlayerState::Dead:
            return "idle_down";
        default:
            return player.isMoving() ? "walk_" + dir : "idle_down";
    }
}

void PlayerRenderer::update(Player& player, float deltaTime) {
    std::string clip = clipForState(player);
    if (clip != lastClip) {
        anim.play(clip);
        lastClip = clip;
    }

    anim.update(deltaTime);
    anim.applyToSprite(sprite, 2.f);

    sprite.setPosition(player.getPosition());
    player.getSprite() = sprite;
}

void PlayerRenderer::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}
