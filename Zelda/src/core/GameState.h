#pragma once

enum class GameState {
    MainMenu,
    NameEntry,
    Controls,
    Instructions,
    Statistics,
    Playing,
    Paused,
    Shop,
    GameOver,
    Victory
};

inline bool isMenuState(GameState state) {
    return state == GameState::MainMenu ||
           state == GameState::NameEntry ||
           state == GameState::Controls ||
           state == GameState::Instructions ||
           state == GameState::Statistics;
}
