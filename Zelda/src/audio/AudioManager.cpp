#include "AudioManager.h"

AudioManager& AudioManager::instance() {
    static AudioManager mgr;
    return mgr;
}

void AudioManager::load() {}
void AudioManager::playMusic(const std::string&, bool) {}
void AudioManager::playSound(const std::string&) {}
void AudioManager::setVolume(float, float sfxVol) { sfxVolume = sfxVol; }
