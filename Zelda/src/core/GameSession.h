#pragma once

struct SaveData;
struct RunTracker;

// Thin orchestrator — wires systems, no gameplay spaghetti.
class GameSession {
public:
    void run();

private:
    void setupEvents(SaveData& saveData);
};
