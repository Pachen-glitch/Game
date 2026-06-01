#pragma once

namespace Constants {

    constexpr int TILE_SIZE = 48;
    constexpr int ROOM_TILES_W = 20;
    constexpr int ROOM_TILES_H = 20;

    constexpr float PLAYER_SPEED = 200.f;
    constexpr float ENEMY_SPEED = 90.f;
    constexpr float BAT_SPEED = 140.f;

    constexpr float ATTACK_DURATION = 0.32f;
    constexpr float SWORD_COOLDOWN = 0.35f;
    constexpr float ATTACK_HIT_FRAME_TIME = 0.09f;
    constexpr float HIT_PAUSE_DURATION = 0.05f;
    constexpr float SPIN_DURATION = 0.6f;
    constexpr float SHIELD_BLOCK_ANGLE = 90.f;

    constexpr float INVULN_DURATION = 1.0f;
    constexpr float KNOCKBACK_FORCE = 120.f;

    constexpr float AGGRO_RADIUS = 160.f;
    constexpr float DEAGGRO_RADIUS = 240.f;
    constexpr float ENEMY_ATTACK_RANGE = 40.f;

    constexpr int STARTING_HEARTS = 7;
    constexpr int MAX_HEARTS = 20;
    constexpr int STARTING_RUPEES = 0;

    constexpr float SHIELD_DAMAGE_MULT = 0.3f;
    constexpr float SHIELD_MOVE_MULT = 0.55f;

    constexpr float BERSERK_DURATION = 10.f;
    constexpr float BERSERK_COOLDOWN = 12.f;
    constexpr float BERSERK_DAMAGE_MULT = 1.5f;
    constexpr float BERSERK_MOVE_MULT = 1.25f;
    constexpr float BERSERK_ATTACK_SPEED_MULT = 1.35f;

    constexpr float SHOP_EXIT_COOLDOWN = 3.f;

    constexpr int MIN_ROOMS = 8;
    constexpr int MAX_ROOMS = 14;

    constexpr int KEYS_REQUIRED_FOR_BOSS = 3;
    constexpr int MAX_FLOORS = 10;

    constexpr unsigned WINDOW_W = 1280;
    constexpr unsigned WINDOW_H = 720;

    constexpr float CAMERA_LERP = 8.f;
}
