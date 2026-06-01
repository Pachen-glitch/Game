#include "Animation.h"

void Animation::addFrame(const std::string& texturePath, float duration) {
    frames.push_back({texturePath, duration});
}
//Devuelve el path del frame actual
const std::string& Animation::getCurrentFramePath() const {
    static const std::string empty;
    if (frames.empty()) return empty;
    return frames[currentIndex].path;
}
//Resetea la animacion
void Animation::reset() {
    currentIndex = 0;
    timer = 0.f;
    finished = false;
}
//Carga el siguiente frame de la animacion
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
// Actualiza la animacion
void Animation::update(float dt) {
    if (frames.empty() || finished) return;

    timer += dt;
    if (timer >= frames[currentIndex].duration) {
        advanceFrame();
    }
}
