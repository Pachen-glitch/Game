#include "Camera.h"

#include <cmath>
#include "../core/Constants.h"
#include "../entity/player/Player.h"

void Camera::update(float dt, const Player& player, sf::Vector2u windowSize) {
    sf::Vector2f target = player.getPosition() +
        player.getSize() * 0.5f;

    float t = 1.f - std::exp(-Constants::CAMERA_LERP * dt);
    position += (target - position) * t;

    view.setSize(static_cast<float>(windowSize.x),
                 static_cast<float>(windowSize.y));
    view.setCenter(position);
}
