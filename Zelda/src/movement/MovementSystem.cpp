#include "MovementSystem.h"

#include "../combat/CombatSystem.h"
#include "../core/Constants.h"
#include "CollisionSystem.h"

#include <SFML/Window/Keyboard.hpp>

#include "../utils/MathUtils.h"

void MovementSystem::update(float dt, Player& player, const Map& map) {
    using namespace sf;

    if (player.getState() == PlayerState::Dead) return;

    bool shield = Keyboard::isKeyPressed(Keyboard::V);
    player.setShieldHeld(shield);

    if (Keyboard::isKeyPressed(Keyboard::F) && !attackPressed) {
        player.trySwordAttack();
        attackPressed = true;
    }
    if (!Keyboard::isKeyPressed(Keyboard::F)) attackPressed = false;

    if (Keyboard::isKeyPressed(Keyboard::X) && !spinPressed) {
        player.trySpinAttack();
        spinPressed = true;
    }
    if (!Keyboard::isKeyPressed(Keyboard::X)) spinPressed = false;

    if (!player.canMove()) {
        player.moving = false;
        player.setVelocity({0.f, 0.f});
        return;
    }

    sf::Vector2f input(0.f, 0.f);
    if (Keyboard::isKeyPressed(Keyboard::W)) input.y -= 1.f;
    if (Keyboard::isKeyPressed(Keyboard::S)) input.y += 1.f;
    if (Keyboard::isKeyPressed(Keyboard::A)) input.x -= 1.f;
    if (Keyboard::isKeyPressed(Keyboard::D)) input.x += 1.f;

    bool hasInput = (input.x != 0.f || input.y != 0.f);
    player.updateLocomotionState(hasInput);

    if (hasInput) {
        input = MathUtils::normalize(input);
        if (std::abs(input.x) > std::abs(input.y)) {
            player.setDirection(input.x > 0 ? Direction::RIGHT : Direction::LEFT);
        } else {
            player.setDirection(input.y > 0 ? Direction::DOWN : Direction::UP);
        }
    }

    float speed = player.getMoveSpeed();
    sf::Vector2f vel = input * speed;
    player.setVelocity(vel);

    sf::FloatRect bounds = player.getBounds();
    sf::Vector2f from(bounds.left, bounds.top);
    sf::Vector2f to = from + vel * dt;
    sf::Vector2f resolved = CollisionSystem::resolveMovement(map,from,to,{bounds.width, bounds.height});
    float offsetX = (player.getSize().x - bounds.width) * 0.5f;
    float offsetY = player.getSize().y - bounds.height;player.setPosition({resolved.x - offsetX,resolved.y - offsetY});
}
