#include "PlayerMovement.h"
#include "../core/Constants.h"

void PlayerMovement::move(Player& player, char input, Map& map) {
    sf::Vector2f pos = player.getPosition();
    int tx = static_cast<int>(pos.x) / Constants::TILE_SIZE;
    int ty = static_cast<int>(pos.y) / Constants::TILE_SIZE;

    switch (input) {
        case 'w': ty--; player.setDirection(Direction::UP); break;
        case 's': ty++; player.setDirection(Direction::DOWN); break;
        case 'a': tx--; player.setDirection(Direction::LEFT); break;
        case 'd': tx++; player.setDirection(Direction::RIGHT); break;
    }

    if (map.getTile(tx, ty) == TileType::WALL) return;

    player.setPosition({
        static_cast<float>(tx * Constants::TILE_SIZE),
        static_cast<float>(ty * Constants::TILE_SIZE)
    });
    player.moving = true;
}
