#pragma once

#include <string>

// Audio opcional — sin enlazar sfml-audio evita DLL faltantes al arrancar.
class AudioManager {
public:
    static AudioManager& instance();

    void load();
    void playMusic(const std::string& path, bool loop = true);
    void playSound(const std::string& name);
    void setVolume(float musicVol, float sfxVol);

private:
    AudioManager() = default;
    float sfxVolume = 80.f;
};
