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

    if (Keyboard::isKeyPressed(Keyboard::C) && !attackPressed) {
        if (player.trySwordAttack()) {
            // CombatSystem spawns hitbox from GameSession when attack starts
        }
        attackPressed = true;
    }
    if (!Keyboard::isKeyPressed(Keyboard::C)) attackPressed = false;

    if (Keyboard::isKeyPressed(Keyboard::X) && !spinPressed) {
        player.trySpinAttack();
        spinPressed = true;
    }
    if (!Keyboard::isKeyPressed(Keyboard::X)) spinPressed = false;

    if (player.getState() == PlayerState::Shield ||
        player.getState() == PlayerState::Attack ||
        player.getState() == PlayerState::Spin ||
        player.getState() == PlayerState::Hurt) {
        return;
    }

    sf::Vector2f input(0.f, 0.f);
    if (Keyboard::isKeyPressed(Keyboard::W)) input.y -= 1.f;
    if (Keyboard::isKeyPressed(Keyboard::S)) input.y += 1.f;
    if (Keyboard::isKeyPressed(Keyboard::A)) input.x -= 1.f;
    if (Keyboard::isKeyPressed(Keyboard::D)) input.x += 1.f;

    player.moving = (input.x != 0.f || input.y != 0.f);

    if (player.moving) {
        input = MathUtils::normalize(input);
        if (std::abs(input.x) > std::abs(input.y)) {
            player.setDirection(input.x > 0 ? Direction::RIGHT : Direction::LEFT);
        } else {
            player.setDirection(input.y > 0 ? Direction::DOWN : Direction::UP);
        }
    }

    float speed = Constants::PLAYER_SPEED * player.getStats().moveSpeedMult;
    sf::Vector2f vel = input * speed;
    player.setVelocity(vel);

    sf::Vector2f from = player.getPosition();
    sf::Vector2f to = from + vel * dt;
    sf::Vector2f resolved = CollisionSystem::resolveMovement(
        map, from, to, player.getSize()
    );
    player.setPosition(resolved);
}
