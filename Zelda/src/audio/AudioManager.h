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
    void playShopMusic();
    void resumeGameplayMusic();
    void playGameOverMusic();

    // --- Future Naruto boss hooks (not wired to gameplay yet) ---
    void playBossSpawnMusic();
    void playBossPreBattleMusic();
    void playBossBattleMusic();
    void playBossDeathMusic();

    void playPauseMusic();
    void resumeFromPause();

    // --- Future special attack sounds (registered, not used yet) ---
    void playRasenganSound();
    void playOdamaSound();
    void playRasenShurikenSound();
    void playNarutoPhaseTransitionSound();

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
        BossDeath,
        Pause
    };

    AudioManager() = default;

    bool playMusicFile(const std::string& relativeFromAudio, bool loop);
    void playCurrentGameplayTrack();
    void advanceGameplayTrack();
    void reshuffleGameplayPlaylist();

    sf::Music music;
    sf::Music rasenganSfx;
    sf::Music odamaSfx;
    sf::Music rasenShurikenSfx;
    sf::Sound sfx;
    sf::SoundBuffer sfxBuffer;

    std::unordered_map<std::string, sf::SoundBuffer> soundBuffers;

    MusicContext context = MusicContext::None;
    MusicContext pausedFromContext = MusicContext::None;
    sf::Time pausedMusicOffset = sf::Time::Zero;
    std::string pauseMusicFile;

    std::vector<std::string> gameplayTracks;
    std::vector<std::string> shuffledPlaylist;
    size_t playlistIndex = 0;
    std::string lastGameplayTrack;

    // Preloaded boss attack streams (avoid openFromFile delay on spawn).
    std::string rasenganPath;
    std::string odamaPath;
    std::string rasenShurikenPath;
    bool rasenganSfxReady = false;
    bool odamaSfxReady = false;
    bool rasenShurikenSfxReady = false;

    // Temporary — cap projectile SFX at 4s until collision/despawn hooks exist.
    float rasenganProjectileSfxTimer = 0.f;
    float odamaProjectileSfxTimer = 0.f;
    float rasenShurikenProjectileSfxTimer = 0.f;

    float musicVolume = 70.f;
    float sfxVolume = 80.f;
    bool loaded = false;
};
