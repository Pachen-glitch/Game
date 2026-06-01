#include "Camera.h"

#include <algorithm>
#include <cmath>

#include "../core/Constants.h"
#include "../core/CombatFeel.h"
#include "../entity/player/Player.h"
#include "../map/Map.h"

void Camera::update(
    float dt,
    const Player& player,
    const Map& map,
    sf::Vector2u windowSize
) {
    sf::Vector2f target = player.getPosition() + player.getSize() * 0.5f;

    float t = 1.f - std::exp(-Constants::CAMERA_LERP * dt);
    position += (target - position) * t;

    const float roomW = static_cast<float>(map.getWidth() * Constants::TILE_SIZE);
    const float roomH = static_cast<float>(map.getHeight() * Constants::TILE_SIZE);
    const float halfW = static_cast<float>(windowSize.x) * 0.5f;
    const float halfH = static_cast<float>(windowSize.y) * 0.5f;

    if (roomW > windowSize.x) {
        position.x = std::clamp(position.x, halfW, roomW - halfW);
    } else {
        position.x = roomW * 0.5f;
    }

    if (roomH > windowSize.y) {
        position.y = std::clamp(position.y, halfH, roomH - halfH);
    } else {
        position.y = roomH * 0.5f;
    }

    sf::Vector2f shake = CombatFeel::instance().getShakeOffset();
    view.setSize(static_cast<float>(windowSize.x),
                 static_cast<float>(windowSize.y));
    view.setCenter(position + shake);
}
