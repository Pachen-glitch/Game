#pragma once

namespace Constants {

    constexpr int TILE_SIZE = 48;
    constexpr int ROOM_TILES_W = 20;
    constexpr int ROOM_TILES_H = 20;

    constexpr float PLAYER_SPEED = 200.f;
    constexpr float ENEMY_SPEED = 90.f;
    constexpr float BAT_SPEED = 140.f;

    constexpr float ATTACK_DURATION = 0.35f;
    constexpr float SPIN_DURATION = 0.6f;
    constexpr float SHIELD_BLOCK_ANGLE = 90.f;

    constexpr float INVULN_DURATION = 1.0f;
    constexpr float KNOCKBACK_FORCE = 120.f;

    constexpr float AGGRO_RADIUS = 160.f;
    constexpr float DEAGGRO_RADIUS = 240.f;
    constexpr float ENEMY_ATTACK_RANGE = 40.f;

    constexpr int STARTING_HEARTS = 3;
    constexpr int MAX_HEARTS = 20;
    constexpr int STARTING_RUPEES = 0;

    constexpr int MIN_ROOMS = 8;
    constexpr int MAX_ROOMS = 14;

    constexpr unsigned WINDOW_W = 1280;
    constexpr unsigned WINDOW_H = 720;

    constexpr float CAMERA_LERP = 8.f;
}
