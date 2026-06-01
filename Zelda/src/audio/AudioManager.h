#pragma once

#include <SFML/Audio/Music.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

#include <string>
#include <unordered_map>
#include <vector>

// Centralized music + SFX — state machine, playlists, boss hooks.
class AudioManager {
public:
    static AudioManager& instance();

    void load();
    void update(float dt);

    // --- High-level music API (GameSession calls these) ---
    void playMenuMusic();
    void startGameplayMusic();
    void enterShop();
    void resumeGameplayMusic();
    void playGameOverMusic();

    // --- Future Naruto boss hooks (not wired to gameplay yet) ---
    void playBossSpawnMusic();
    void playBossPreBattleMusic();
    void playBossBattleMusic();
    void playBossDeathMusic();

    // --- Future special attack sounds (registered, not used yet) ---
    void playRasenganSound();
    void playRasenShurikenSound();

    void playSound(const std::string& name);
    void setVolume(float musicVol, float sfxVol);

private:
    enum class MusicContext {
        None,
        Menu,
        Gameplay,
        Shop,
        GameOver,
        BossSpawn,
        BossPreBattle,
        BossBattle,
        BossDeath
    };

    AudioManager() = default;

    bool playMusicFile(const std::string& relativeFromAudio, bool loop);
    void playCurrentGameplayTrack();
    void advanceGameplayTrack();
    void reshuffleGameplayPlaylist();

    sf::Music music;
    sf::Sound sfx;
    sf::SoundBuffer sfxBuffer;

    std::unordered_map<std::string, sf::SoundBuffer> soundBuffers;

    MusicContext context = MusicContext::None;

    std::vector<std::string> gameplayTracks;
    std::vector<std::string> shuffledPlaylist;
    size_t playlistIndex = 0;
    std::string lastGameplayTrack;

    // Registered paths for future boss attack sounds.
    std::string rasenganPath;
    std::string rasenShurikenPath;

    float musicVolume = 70.f;
    float sfxVolume = 80.f;
    bool loaded = false;
};
