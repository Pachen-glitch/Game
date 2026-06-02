#include "AudioManager.h"

#include "../utils/AssetPaths.h"

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <random>
#include <string>
#include <vector>

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

// Temporary polish caps — stop when projectile collision/despawn hooks exist.
constexpr float kRasenganProjectileSfxLifetime = 4.f;
constexpr float kOdamaProjectileSfxLifetime = 3.f;
constexpr float kRasenShurikenProjectileSfxLifetime = 2.5f;

std::string resolveExistingAudio(const std::vector<const char*>& candidates) {
    const std::string& root = AssetPaths::assetsRoot();
    for (const char* candidate : candidates) {
        std::string abs = root + "/audio/" + candidate;
        if (std::filesystem::exists(abs)) {
            return abs;
        }
    }
    return {};
}

} // namespace

void AudioManager::load() {
    if (loaded) return;

    gameplayTracks = {
        "music/Game_1.mp3",
        "music/Game_2.mp3",
        "music/Game_3.mp3"
    };
// Carga los SFX
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

    rasenganPath = resolveExistingAudio({
        "naruto/Rasengan_1.mp3",
        "naruto/Odama_1.mp3",
        "naruto/Odama.mp3",
        "naruto/odama_1.mp3",
    });
    rasenShurikenPath = resolveExistingAudio({
        "naruto/RasenShuriken_1.mp3",
        "naruto/RasenShuriken.mp3",
    });
    odamaPath = resolveExistingAudio({
        "naruto/Odama_1.mp3",
        "naruto/Odama.mp3",
        "naruto/odama_1.mp3",
    });

    if (!rasenganPath.empty()) {
        rasenganSfxReady = rasenganSfx.openFromFile(rasenganPath);
        if (rasenganSfxReady) {
            rasenganSfx.setLoop(false);
            rasenganSfx.setVolume(sfxVolume);
        }
    } else {
        std::cerr << "[AudioManager] Rasengan SFX not found — add assets/audio/naruto/Rasengan_1.mp3\n";
    }
// Carga el SFX de Odama
    if (!odamaPath.empty()) {
        odamaSfxReady = odamaSfx.openFromFile(odamaPath);
        if (odamaSfxReady) {
            odamaSfx.setLoop(false);
            odamaSfx.setVolume(sfxVolume);
        }
    } else {
        std::cerr << "[AudioManager] Odama SFX not found — add assets/audio/naruto/Odama_1.mp3\n";
    }

    if (!rasenShurikenPath.empty()) {
        rasenShurikenSfxReady = rasenShurikenSfx.openFromFile(rasenShurikenPath);
        if (rasenShurikenSfxReady) {
            rasenShurikenSfx.setLoop(false);
            rasenShurikenSfx.setVolume(sfxVolume);
        }
    }
// Configura el volumen de la musica y los SFX
    music.setVolume(musicVolume);
    sfx.setVolume(sfxVolume);

    loaded = true;
}

void AudioManager::update(float dt) {
    // Temporary: stop projectile attack SFX after kProjectileSfxLifetime even if
    // the projectile still exists (no wall collision / despawn audio yet).
    if (rasenganProjectileSfxTimer > 0.f) {
        rasenganProjectileSfxTimer -= dt;
        if (rasenganProjectileSfxTimer <= 0.f) {
            rasenganProjectileSfxTimer = 0.f;
            if (rasenganSfxReady) {
                rasenganSfx.stop();
            }
        }
    }

    if (odamaProjectileSfxTimer > 0.f) {
        odamaProjectileSfxTimer -= dt;
        if (odamaProjectileSfxTimer <= 0.f) {
            odamaProjectileSfxTimer = 0.f;
            if (odamaSfxReady) {
                odamaSfx.stop();
            }
        }
    }

    if (rasenShurikenProjectileSfxTimer > 0.f) {
        rasenShurikenProjectileSfxTimer -= dt;
        if (rasenShurikenProjectileSfxTimer <= 0.f) {
            rasenShurikenProjectileSfxTimer = 0.f;
            if (rasenShurikenSfxReady) {
                rasenShurikenSfx.stop();
            }
        }
    }

    if (context != MusicContext::Gameplay) return;
    if (music.getStatus() != sf::Music::Stopped) return;

    advanceGameplayTrack();
}
// Reproduce la musica
bool AudioManager::playMusicFile(const std::string& relativeFromAudio, bool loop) {
    std::string path = AssetPaths::getAudioPath(relativeFromAudio);
    if (!openMusic(music, path, loop, musicVolume)) {
        return false;
    }
    return true;
}
// Mezcla la playlist de la musica
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
// Reproduce la siguiente pista de la playlist
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
// Reproduce la playlist de la musica
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
    if (context == MusicContext::Shop) {
        context = MusicContext::Gameplay;
        advanceGameplayTrack();
        return;
    }

    if (context == MusicContext::BossPreBattle) {
        context = MusicContext::Gameplay;
        if (shuffledPlaylist.empty()) {
            reshuffleGameplayPlaylist();
        }
        playCurrentGameplayTrack();
    }
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
// Reproduce la musica de pre-batalla
void AudioManager::playBossPreBattleMusic() {
    if (context == MusicContext::BossPreBattle) return;

    context = MusicContext::BossPreBattle;

    static const char* kCandidates[] = {
        "music/Naruto_PreBattle.mp3",
        "music/Naruto_PreBatle.mp3",
    };

    for (const char* track : kCandidates) {
        if (playMusicFile(track, true)) return;
    }
}

void AudioManager::playShopMusic() {
    context = MusicContext::Shop;

    music.stop();

    music.openFromFile(
        AssetPaths::getAudioPath(
            "music/shop_music.mp3"
        )
    );

    music.setLoop(true);

    music.play();
}

void AudioManager::playBossBattleMusic() {
    context = MusicContext::BossBattle;
    playMusicFile("music/Naruto_Batle.mp3", true);
}

void AudioManager::playBossDeathMusic() {
    context = MusicContext::BossDeath;
    playMusicFile("music/Naruto_Dead.mp3", false);
}

void AudioManager::playPauseMusic() {
    if (context == MusicContext::Pause) return;

    pausedFromContext = context;
    if (music.getStatus() == sf::Music::Playing ||
        music.getStatus() == sf::Music::Paused) {
        pausedMusicOffset = music.getPlayingOffset();
    } else {
        pausedMusicOffset = sf::Time::Zero;
    }

    context = MusicContext::Pause;
    music.stop();
    playMusicFile("music/Pause.mp3", true);
}

void AudioManager::resumeFromPause() {
    if (context != MusicContext::Pause) return;

    const MusicContext restore = pausedFromContext;
    const sf::Time offset = pausedMusicOffset;

    music.stop();
    context = restore;
    pausedFromContext = MusicContext::None;
    pausedMusicOffset = sf::Time::Zero;

    switch (restore) {
        case MusicContext::Gameplay:
            if (shuffledPlaylist.empty()) {
                reshuffleGameplayPlaylist();
            }
            if (!shuffledPlaylist.empty()) {
                playMusicFile(shuffledPlaylist[playlistIndex], false);
                if (offset.asMilliseconds() > 0) {
                    music.setPlayingOffset(offset);
                }
            }
            break;
        case MusicContext::BossPreBattle:
            context = MusicContext::None;
            playBossPreBattleMusic();
            if (offset.asMilliseconds() > 0) {
                music.setPlayingOffset(offset);
            }
            break;
        case MusicContext::BossBattle:
            playBossBattleMusic();
            if (offset.asMilliseconds() > 0) {
                music.setPlayingOffset(offset);
            }
            break;
        case MusicContext::BossSpawn:
            playBossSpawnMusic();
            if (offset.asMilliseconds() > 0) {
                music.setPlayingOffset(offset);
            }
            break;
        case MusicContext::BossDeath:
            playBossDeathMusic();
            if (offset.asMilliseconds() > 0) {
                music.setPlayingOffset(offset);
            }
            break;
        case MusicContext::Shop:
            enterShop();
            break;
        case MusicContext::GameOver:
            playGameOverMusic();
            break;
        default:
            break;
    }
}

void AudioManager::playNarutoPhaseTransitionSound() {
    if (!rasenganSfxReady) return;
    rasenganSfx.stop();
    rasenganSfx.setPlayingOffset(sf::Time::Zero);
    rasenganSfx.setVolume(sfxVolume * 0.85f);
    rasenganSfx.play();
}

void AudioManager::playRasenganSound() {
    if (!rasenganSfxReady) return;
    rasenganSfx.stop();
    rasenganSfx.setPlayingOffset(sf::Time::Zero);
    rasenganSfx.setVolume(sfxVolume);
    rasenganSfx.play();
    rasenganProjectileSfxTimer = kRasenganProjectileSfxLifetime;
}

void AudioManager::playOdamaSound() {
    if (!odamaSfxReady) return;
    odamaSfx.stop();
    odamaSfx.setPlayingOffset(sf::Time::Zero);
    odamaSfx.setVolume(sfxVolume);
    odamaSfx.play();
    odamaProjectileSfxTimer = kOdamaProjectileSfxLifetime;
}

void AudioManager::playRasenShurikenSound() {
    if (!rasenShurikenSfxReady) return;
    rasenShurikenSfx.stop();
    rasenShurikenSfx.setPlayingOffset(sf::Time::Zero);
    rasenShurikenSfx.setVolume(sfxVolume);
    rasenShurikenSfx.play();
    rasenShurikenProjectileSfxTimer = kRasenShurikenProjectileSfxLifetime;
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
    if (rasenganSfxReady) {
        rasenganSfx.setVolume(sfxVolume);
    }
    if (odamaSfxReady) {
        odamaSfx.setVolume(sfxVolume);
    }
    if (rasenShurikenSfxReady) {
        rasenShurikenSfx.setVolume(sfxVolume);
    }
}
