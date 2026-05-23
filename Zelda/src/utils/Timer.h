#pragma once

class Timer {
public:
    void start(float dur) {
        duration = dur;
        remaining = dur;
        active = dur > 0.f;
    }

    void tick(float dt) {
        if (!active) return;
        remaining -= dt;
        if (remaining <= 0.f) {
            remaining = 0.f;
            active = false;
        }
    }

    bool isActive() const { return active; }
    bool finished() const { return !active; }
    float getRemaining() const { return remaining; }
    float getDuration() const { return duration; }

    void reset() { active = false; remaining = 0.f; }

private:
    float duration = 0.f;
    float remaining = 0.f;
    bool active = false;
};
