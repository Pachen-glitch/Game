#include "AudioManager.h"

#include "../utils/AssetPaths.h"

#include <algorithm>
#include <iostream>
#include <random>

AudioManager& AudioManager::instance() {
    static AudioManager mgr;
    return mgr;
}

namespace {

bool openMusic(sf::Music& player, const std::string& path, bool loop, float volume) {
    if (path.empty()) return false;
    if (!player.openFromFile(path)) {
        std::cerr << "[AudioManager] Failed to open music: " << path << "\n";
        return false;
    }
    player.setLoop(loop);
    player.setVolume(volume);
    player.play();
    return true;
}

} // namespace

void AudioManager::load() {
    if (loaded) return;

    gameplayTracks = {
        "music/Game_1.mp3",
        "music/Game_2.mp3",
        "music/Game_3.mp3"
    };

    auto loadSfx = [this](const std::string& name, const std::string& file) {
        std::string path = AssetPaths::getAudioPath(file);
        if (path.empty()) return;
        sf::SoundBuffer buffer;
        if (buffer.loadFromFile(path)) {
            soundBuffers[name] = std::move(buffer);
        }
    };

    loadSfx("attack", "attack.wav");
    loadSfx("pickup", "pickup.wav");
    loadSfx("enemy_death", "enemy_death.wav");
    loadSfx("hurt", "hurt.wav");

    rasenganPath = AssetPaths::getAudioPath("naruto/Rasengan_1.mp3");
    rasenShurikenPath = AssetPaths::getAudioPath("naruto/RasenShuriken_1.mp3");

    music.setVolume(musicVolume);
    sfx.setVolume(sfxVolume);

    loaded = true;
}

void AudioManager::update(float dt) {
    (void)dt;

    if (context != MusicContext::Gameplay) return;
    if (music.getStatus() != sf::Music::Stopped) return;

    advanceGameplayTrack();
}

bool AudioManager::playMusicFile(const std::string& relativeFromAudio, bool loop) {
    std::string path = AssetPaths::getAudioPath(relativeFromAudio);
    if (!openMusic(music, path, loop, musicVolume)) {
        return false;
    }
    return true;
}

void AudioManager::reshuffleGameplayPlaylist() {
    shuffledPlaylist = gameplayTracks;
    if (shuffledPlaylist.empty()) return;

    static std::mt19937 rng{std::random_device{}()};
    std::shuffle(shuffledPlaylist.begin(), shuffledPlaylist.end(), rng);

    if (shuffledPlaylist.size() > 1 &&
        shuffledPlaylist.front() == lastGameplayTrack) {
        std::swap(shuffledPlaylist.front(), shuffledPlaylist[1]);
    }

    playlistIndex = 0;
}

void AudioManager::playCurrentGameplayTrack() {
    if (shuffledPlaylist.empty()) return;

    const std::string& track = shuffledPlaylist[playlistIndex];
    if (playMusicFile(track, false)) {
        lastGameplayTrack = track;
    }
}

void AudioManager::advanceGameplayTrack() {
    if (gameplayTracks.empty()) return;

    playlistIndex++;
    if (playlistIndex >= shuffledPlaylist.size()) {
        reshuffleGameplayPlaylist();
    }
    playCurrentGameplayTrack();
}

void AudioManager::playMenuMusic() {
    context = MusicContext::Menu;
    playMusicFile("music/Menu.mp3", true);
}

void AudioManager::startGameplayMusic() {
    context = MusicContext::Gameplay;
    reshuffleGameplayPlaylist();
    playCurrentGameplayTrack();
}

void AudioManager::enterShop() {
    context = MusicContext::Shop;

    static std::mt19937 rng{std::random_device{}()};
    std::uniform_int_distribution<int> pick(0, 1);
    const char* shopTrack = (pick(rng) == 0) ? "music/Shop.mp3" : "music/Shop_2.mp3";
    playMusicFile(shopTrack, true);
}

void AudioManager::resumeGameplayMusic() {
    if (context != MusicContext::Shop) return;

    context = MusicContext::Gameplay;
    advanceGameplayTrack();
}

void AudioManager::playGameOverMusic() {
    context = MusicContext::GameOver;
    music.stop();
    playMusicFile("music/GameOver.mp3", false);
}

void AudioManager::playBossSpawnMusic() {
    context = MusicContext::BossSpawn;
    playMusicFile("music/Boss_Spawn.mp3", false);
}

void AudioManager::playBossPreBattleMusic() {
    context = MusicContext::BossPreBattle;
    playMusicFile("music/Naruto_PreBatle.mp3", true);
}

void AudioManager::playBossBattleMusic() {
    context = MusicContext::BossBattle;
    playMusicFile("music/Naruto_Batle.mp3", true);
}

void AudioManager::playBossDeathMusic() {
    context = MusicContext::BossDeath;
    playMusicFile("music/Naruto_Dead.mp3", false);
}

void AudioManager::playRasenganSound() {
    // Registered path: naruto/Rasengan_1.mp3 — wire during Naruto boss attacks.
    (void)rasenganPath;
}

void AudioManager::playRasenShurikenSound() {
    // Registered path: naruto/RasenShuriken_1.mp3 — wire during Naruto boss attacks.
    (void)rasenShurikenPath;
}

void AudioManager::playSound(const std::string& name) {
    auto it = soundBuffers.find(name);
    if (it == soundBuffers.end()) return;

    sfx.setBuffer(it->second);
    sfx.setVolume(sfxVolume);
    sfx.play();
}

void AudioManager::setVolume(float musicVol, float sfxVol) {
    musicVolume = musicVol;
    sfxVolume = sfxVol;
    music.setVolume(musicVolume);
    sfx.setVolume(sfxVolume);
}
