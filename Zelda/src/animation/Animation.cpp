#include "Animation.h"

void Animation::addFrame(const std::string& texturePath, float duration) {
    frames.push_back({texturePath, duration});
}

const std::string& Animation::getCurrentFramePath() const {
    static const std::string empty;
    if (frames.empty()) return empty;
    return frames[currentIndex].path;
}

void Animation::reset() {
    currentIndex = 0;
    timer = 0.f;
    finished = false;
}

void Animation::advanceFrame() {
    if (frames.empty()) return;

    currentIndex++;
    if (currentIndex >= static_cast<int>(frames.size())) {
        if (looping) {
            currentIndex = 0;
        } else {
            currentIndex = static_cast<int>(frames.size()) - 1;
            finished = true;
        }
    }
    timer = 0.f;
}

void Animation::update(float dt) {
    if (frames.empty() || finished) return;

    timer += dt;
    if (timer >= frames[currentIndex].duration) {
        advanceFrame();
    }
}
