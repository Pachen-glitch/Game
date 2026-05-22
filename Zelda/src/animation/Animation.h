#pragma once

#include <SFML/Graphics/Texture.hpp>
#include <string>
#include <vector>

// One animation clip: ordenaod frames + timing.
class Animation {
public:
    Animation() = default;

    void addFrame(const std::string& texturePath, float duration);
    void setLooping(bool loop) { looping = loop; }

    const std::string& getCurrentFramePath() const;
    bool isFinished() const { return finished; }
    void reset();

    void update(float dt);
    void advanceFrame();

private:
    struct Frame {
        std::string path;
        float duration;
    };

    std::vector<Frame> frames;
    int currentIndex = 0;
    float timer = 0.f;
    bool looping = true;
    bool finished = false;
};
