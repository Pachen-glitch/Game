#include "TilemapRenderer.h"
#include "TextureCache.h"
#include "../core/Constants.h"
#include "../map/Map.h"


TilemapRenderer::TilemapRenderer()
{// Constructor
    floorTexture = &TextureCache::instance().get(
        "assets/sprites/map/floor/floor.png"
    );
// Carga el texture para el wall top
    wallTopTexture = &TextureCache::instance().get(
        "assets/sprites/map/walls/wall_top.png"
    );

    wallBottomTexture = &TextureCache::instance().get(
        "assets/sprites/map/walls/wall_bottom.png"
    );

    wallLeftTexture = &TextureCache::instance().get(
        "assets/sprites/map/walls/wall_left.png"
    );

    wallRightTexture = &TextureCache::instance().get(
        "assets/sprites/map/walls/wall_right.png"
    );

    cornerTLTexture = &TextureCache::instance().get(
        "assets/sprites/map/walls/corner_tl.png"
    );

    cornerTRTexture = &TextureCache::instance().get(
        "assets/sprites/map/walls/corner_tr.png"
    );

    cornerBLTexture = &TextureCache::instance().get(
        "assets/sprites/map/walls/corner_bl.png"
    );

    cornerBRTexture = &TextureCache::instance().get(
        "assets/sprites/map/walls/corner_br.png"
    );


    doorTexture = &TextureCache::instance().get(
        "assets/sprites/objects/door/door_ud.png"
    );

    columnTexture = &TextureCache::instance().get(
        "assets/sprites/map/style/column.png"
    );

    fireFrames.push_back(
        &TextureCache::instance().get(
            "assets/sprites/map/style/statue_1.png"
        )
    );

    fireFrames.push_back(
        &TextureCache::instance().get(
            "assets/sprites/map/style/statue_2.png"
        )
    );

    fireFrames.push_back(
        &TextureCache::instance().get(
            "assets/sprites/map/style/statue_3.png"
        )
    );

    fireFrames.push_back(
        &TextureCache::instance().get(
            "assets/sprites/map/style/statue_4.png"
        )
    );

    fireFrames.push_back(
        &TextureCache::instance().get(
            "assets/sprites/map/style/statue_5.png"
        )
    );

    bushTexture = &TextureCache::instance().get(
        "assets/sprites/map/style/bush.png"
    );

    rockTexture = &TextureCache::instance().get(
        "assets/sprites/map/style/rock.png"
    );

    waterTexture = &TextureCache::instance().get(
        "assets/sprites/map/style/water.png"
    );

    pitTexture = &TextureCache::instance().get(
        "assets/sprites/map/walls/wall_bottom.png"
    );

    sprite.setScale(2.f, 2.f);
}
// Dibuja el tilemap
void TilemapRenderer::draw(sf::RenderWindow& window,
                           const Map& map)
{
    // Dibuja el tilemap
    for (int y = 0; y < map.getHeight(); ++y)
    {
        for (int x = 0; x < map.getWidth(); ++x)
        {
            TileType t = map.getTile(x, y);// Obtiene el tipo de tile

            switch (t)// Switch para el tipo de tile
            {
                case TileType::FLOOR:
                    sprite.setTexture(*floorTexture, true);// Setea el texture para el floor
                    break;

                case TileType::WALL:
                {
                    int width = map.getWidth();
                    int height = map.getHeight();

                    bool top = (y == 0);
                    bool bottom = (y == height - 1);
                    bool left = (x == 0);
                    bool right = (x == width - 1);

                    if (top && left)
                    {
                        sprite.setTexture(*cornerTLTexture, true);
                    }
                    else if (top && right)
                    {
                        sprite.setTexture(*cornerTRTexture, true);
                    }
                    else if (bottom && left)
                    {
                        sprite.setTexture(*cornerBLTexture, true);
                    }
                    else if (bottom && right)
                    {
                        sprite.setTexture(*cornerBRTexture, true);
                    }
                    else if (top)
                    {
                        sprite.setTexture(*wallTopTexture, true);
                    }
                    else if (bottom)
                    {
                        sprite.setTexture(*wallBottomTexture, true);
                    }
                    else if (left)
                    {
                        sprite.setTexture(*wallLeftTexture, true);
                    }
                    else if (right)
                    {
                        sprite.setTexture(*wallRightTexture, true);
                    }
                    else
                    {
                        sprite.setTexture(*wallTopTexture, true);
                    }

                    break;
                }

                case TileType::DOOR:
                case TileType::LOCKED_DOOR:
                    sprite.setTexture(*doorTexture, true);
                    break;

                case TileType::OPENING:
                    sprite.setTexture(*floorTexture, true);
                    break;
                case TileType::TREE:
                    sprite.setTexture(*columnTexture, true);
                    break;

                case TileType::STATUE:
                {
                    int frame =
                    (
                        static_cast<int>(
                            fireClock.getElapsedTime().asSeconds() * 8.f
                        )
                    ) % fireFrames.size();

                    sprite.setTexture(
                        *fireFrames[frame],
                        true
                    );

                    break;
                }

                case TileType::BUSH:
                    sprite.setTexture(*bushTexture, true);
                    break;

                case TileType::ROCK:
                    sprite.setTexture(*rockTexture, true);
                    break;

                case TileType::WATER:
                    sprite.setTexture(*waterTexture, true);
                    break;
                case TileType::PIT:
                    sprite.setTexture(*pitTexture, true);
                    break;    
                default:
                    sprite.setTexture(*floorTexture, true);
                    break;
            }

            sprite.setPosition(
                static_cast<float>(x * Constants::TILE_SIZE),
                static_cast<float>(y * Constants::TILE_SIZE)
            );

            window.draw(sprite);
        }
    }
}