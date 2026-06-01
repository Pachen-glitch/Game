#include "Minimap.h"
#include "../procedural/Room.h"

#include <SFML/Graphics/RectangleShape.hpp>

void Minimap::draw(
    sf::RenderWindow& window,
    const std::vector<Room>& rooms,
    int currentId
) {
    sf::RectangleShape cell;
    cell.setSize({12.f, 12.f});

    float ox = 1100.f;
    float oy = 20.f;

    for (const auto& room : rooms) {
        cell.setPosition(
            ox + room.gridPos.x * 14.f,
            oy + room.gridPos.y * 14.f
        );

        if (!room.visited) {
            cell.setFillColor(sf::Color(40, 40, 40, 120));
        } else if (room.id == currentId) {
            cell.setFillColor(sf::Color::Yellow);
        } else if (room.type == RoomType::Boss) {
            cell.setFillColor(sf::Color::Red);
        } else if (room.type == RoomType::BossAntechamber) {
            cell.setFillColor(sf::Color(255, 140, 40));
        } else if (room.type == RoomType::Shop) {
            cell.setFillColor(sf::Color::Cyan);
        } else {
            cell.setFillColor(sf::Color(80, 80, 120));
        }

        window.draw(cell);
    }
}
