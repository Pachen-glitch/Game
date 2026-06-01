#include "GameSession.h"

#include "CombatFeel.h"
#include "Constants.h"
#include "GameState.h"

#include "../audio/AudioManager.h"
#include "../combat/CombatSystem.h"
#include "../economy/EconomySystem.h"
#include "../entity/player/Player.h"
#include "../entity/player/PlayerState.h"
#include "../interaction/EventBus.h"
#include "../interaction/InteractionSystem.h"
#include "../movement/MovementSystem.h"
#include "../procedural/World.h"
#include "../render/Camera.h"
#include "../render/EntityRenderer.h"
#include "../render/PlayerRenderer.h"
#include "../render/TilemapRenderer.h"
#include "../save/SaveSystem.h"
#include "../save/HighScore.h"
#include "../save/RunScoreTracker.h"
#include "../ui/DamageNumbers.h"
#include "../ui/GameScreens.h"
#include "../ui/HUD.h"
#include "../ui/Minimap.h"
#include "../ui/ShopUI.h"
#include "../ui/menu/ControlsScreen.h"
#include "../ui/menu/InstructionsScreen.h"
#include "../ui/menu/MainMenu.h"
#include "../ui/menu/NameEntryScreen.h"
#include "../ui/menu/StatisticsScreen.h"
#include "../utils/AssetPaths.h"

#include <SFML/Graphics.hpp>
#include <SFML/Window/Keyboard.hpp>

#include <algorithm>
#include <iostream>

namespace {

void startNewRun(
    World& world,
    Player& player,
    SaveSystem& saveSystem,
    SaveData& saveData
) {
    world.newRun();
    player = Player(world.currentRoom().getPlayerSpawn());
    saveSystem.applyToPlayer(player, saveData);
    saveData.runsCompleted++;
}

} // namespace

struct RunTracker {
    RunScoreTracker score;
    std::string playerName = "Aventurero";
    bool scoreSubmitted = false;

    void beginRun(const std::string& name) {
        playerName = normalizePlayerName(name);
        scoreSubmitted = false;
        score.reset();
        RunScoreTracker::setActive(&score);
    }

    void submitScore(SaveData& saveData) {
        if (scoreSubmitted) return;
        scoreSubmitted = true;

        const int finalScore = score.finalizeScore();
        submitHighScore(
            saveData.highScores,
            saveData.bestScore,
            saveData.bestScoreHolder,
            playerName,
            finalScore
        );
        RunScoreTracker::setActive(nullptr);
    }
};

void GameSession::setupEvents(SaveData& saveData) {
    EventBus& bus = EventBus::instance();

    bus.subscribe("player_attack", []() {
        AudioManager::instance().playSound("attack");
    });
    bus.subscribe("rupee_pickup", [&saveData]() {
        AudioManager::instance().playSound("pickup");
        saveData.totalRupeesCollected++;
    });
    bus.subscribe("enemy_hit", []() {
        CombatFeel::instance().triggerHitPause(Constants::HIT_PAUSE_DURATION);
    });
    bus.subscribe("enemy_died", [&saveData]() {
        AudioManager::instance().playSound("enemy_death");
        saveData.enemiesDefeated++;
    });
    bus.subscribe("player_damaged", []() {
        AudioManager::instance().playSound("hurt");
    });
    bus.subscribe("naruto_boss_defeated", []() {
        AudioManager::instance().playBossDeathMusic();
        if (RunScoreTracker* tracker = RunScoreTracker::active()) {
            tracker->onNarutoBossDefeated();
        }
    });
}

void GameSession::run() {
    AssetPaths::initialize();
    AssetPaths::validateCriticalAssets();

    sf::RenderWindow window(
        sf::VideoMode(Constants::WINDOW_W, Constants::WINDOW_H),
        "The Legend of Pachen"
    );
    window.setFramerateLimit(60);

    SaveData saveData;
    SaveSystem saveSystem;
    saveSystem.load(saveData);

    RunTracker runTracker;
    setupEvents(saveData);
    AudioManager::instance().load();
    AudioManager::instance().playMenuMusic();

    World world;
    Player player({0.f, 0.f});

    EventBus::instance().subscribe("boss_gate_unlocked", [&world]() {
        world.unlockBossGate();
    });

    MovementSystem movement;
    CombatSystem combat;
    InteractionSystem interaction;
    EconomySystem economy;

    PlayerRenderer playerRenderer;
    TilemapRenderer tilemap;
    EntityRenderer entityRenderer;
    Camera camera;
    HUD hud;
    Minimap minimap;
    ShopUI shopUI;
    GameScreens screens;
    DamageNumbers damageNumbers;

    MainMenu mainMenu;
    NameEntryScreen nameEntryScreen;
    ControlsScreen controlsScreen;
    InstructionsScreen instructionsScreen;
    StatisticsScreen statisticsScreen;

    sf::Vector2u winSize = window.getSize();
    mainMenu.layoutForWindow(winSize);
    nameEntryScreen.prepare(winSize);
    controlsScreen.prepare(winSize);
    instructionsScreen.prepare(winSize);
    statisticsScreen.prepare(winSize, saveData);

    GameState state = GameState::MainMenu;
    sf::Clock clock;

    EventBus::instance().subscribe("open_shop", [&]() {
        if (state == GameState::Shop) return;
        if (interaction.isShopOnCooldown()) return;
        state = GameState::Shop;
        economy.openShop(player, world.getCurrentRoomId());
        AudioManager::instance().enterShop();
    });

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();

        sf::Event ev;
        while (window.pollEvent(ev)) {
            if (ev.type == sf::Event::Closed) {
                window.close();
                continue;
            }

            if (state == GameState::MainMenu) {
                MainMenu::MenuResult result = mainMenu.handleEvent(ev, window);
                if (result.closeApp) {
                    window.close();
                    continue;
                }
                if (result.nextState != GameState::MainMenu) {
                    if (result.nextState == GameState::NameEntry) {
                        nameEntryScreen.resetInput();
                        nameEntryScreen.prepare(window.getSize());
                    } else if (result.nextState == GameState::Statistics) {
                        statisticsScreen.prepare(window.getSize(), saveData);
                    }
                    state = result.nextState;
                }
            } else if (state == GameState::NameEntry) {
                GameState next = nameEntryScreen.handleEvent(ev, window);
                if (next == GameState::Playing) {
                    runTracker.beginRun(nameEntryScreen.getEnteredName());
                    startNewRun(world, player, saveSystem, saveData);
                    AudioManager::instance().startGameplayMusic();
                } else if (next == GameState::MainMenu) {
                    AudioManager::instance().playMenuMusic();
                }
                state = next;
            } else if (state == GameState::Controls) {
                GameState next = controlsScreen.handleEvent(ev, window);
                if (next == GameState::MainMenu) {
                    AudioManager::instance().playMenuMusic();
                }
                state = next;
            } else if (state == GameState::Instructions) {
                GameState next = instructionsScreen.handleEvent(ev, window);
                if (next == GameState::MainMenu) {
                    AudioManager::instance().playMenuMusic();
                }
                state = next;
            } else if (state == GameState::Statistics) {
                GameState next = statisticsScreen.handleEvent(ev, window);
                if (next == GameState::MainMenu) {
                    AudioManager::instance().playMenuMusic();
                }
                state = next;
            } else if (ev.type == sf::Event::KeyPressed) {
                if (ev.key.code == sf::Keyboard::Escape) {
                    if (state == GameState::Shop) {
                        state = GameState::Playing;
                        interaction.startShopCooldown(Constants::SHOP_EXIT_COOLDOWN);
                        AudioManager::instance().resumeGameplayMusic();
                    } else if (state == GameState::Playing) {
                        state = GameState::Paused;
                    } else if (state == GameState::Paused) {
                        state = GameState::Playing;
                    }
                }
                if (ev.key.code == sf::Keyboard::E &&
                    state == GameState::Playing) {
                    interaction.tryInteract(player, world.getEntities());
                }
                if (ev.key.code == sf::Keyboard::R &&
                    state == GameState::GameOver) {
                    runTracker.beginRun(runTracker.playerName);
                    startNewRun(world, player, saveSystem, saveData);
                    AudioManager::instance().startGameplayMusic();
                    state = GameState::Playing;
                }
                if (ev.key.code == sf::Keyboard::P &&
                    state == GameState::Playing) {
                    sf::Vector2f spawnPos =
                        player.getPosition() + sf::Vector2f(180.f, 0.f);
                    if (world.debugSpawnNarutoNear(spawnPos)) {
                        std::cerr << "[DEBUG] NarutoBoss spawned\n";
                    } else {
                        std::cerr << "[DEBUG] NarutoBoss already exists\n";
                    }
                }
                if (ev.key.code == sf::Keyboard::O &&
                    state == GameState::Playing) {
                    if (world.debugRemoveNaruto()) {
                        std::cerr << "[DEBUG] NarutoBoss removed\n";
                    }
                }
            }

            if (ev.type == sf::Event::MouseMoved && state == GameState::MainMenu) {
                mainMenu.update(sf::Vector2f(
                    static_cast<float>(ev.mouseMove.x),
                    static_cast<float>(ev.mouseMove.y)
                ));
            } else if (ev.type == sf::Event::MouseMoved &&
                       state == GameState::Controls) {
                controlsScreen.update(sf::Vector2f(
                    static_cast<float>(ev.mouseMove.x),
                    static_cast<float>(ev.mouseMove.y)
                ));
            } else if (ev.type == sf::Event::MouseMoved &&
                       state == GameState::Instructions) {
                instructionsScreen.update(sf::Vector2f(
                    static_cast<float>(ev.mouseMove.x),
                    static_cast<float>(ev.mouseMove.y)
                ));
            } else if (ev.type == sf::Event::MouseMoved &&
                       state == GameState::NameEntry) {
                nameEntryScreen.update(sf::Vector2f(
                    static_cast<float>(ev.mouseMove.x),
                    static_cast<float>(ev.mouseMove.y)
                ));
            } else if (ev.type == sf::Event::MouseMoved &&
                       state == GameState::Statistics) {
                statisticsScreen.update(sf::Vector2f(
                    static_cast<float>(ev.mouseMove.x),
                    static_cast<float>(ev.mouseMove.y)
                ));
            }
        }

        if (isMenuState(state)) {
            AudioManager::instance().update(dt);
            window.clear(sf::Color(10, 10, 20));
            switch (state) {
                case GameState::MainMenu:
                    mainMenu.draw(window);
                    break;
                case GameState::NameEntry:
                    mainMenu.draw(window);
                    nameEntryScreen.draw(window);
                    break;
                case GameState::Controls:
                    controlsScreen.draw(window);
                    break;
                case GameState::Instructions:
                    instructionsScreen.draw(window);
                    break;
                case GameState::Statistics:
                    statisticsScreen.draw(window);
                    break;
                default:
                    break;
            }
            window.display();
            continue;
        }

        CombatFeel::instance().tick(dt);
        float gameDt = dt * CombatFeel::instance().getTimeScale();
        AudioManager::instance().update(dt);

        if (state == GameState::Playing) {
            saveData.totalPlayTimeSeconds += gameDt;
            saveData.maxRoomReached = std::max(
                saveData.maxRoomReached,
                world.getCurrentRoomId()
            );
            runTracker.score.addSurvivalTime(gameDt);

            PlayerState prevState = player.getState();
            movement.update(gameDt, player, world.currentRoom().map);

            if (player.getState() == PlayerState::Attack &&
                player.shouldSpawnSwordHit()) {
                combat.spawnSwordHitbox(player);
                player.markSwordHitSpawned();
            }
            if (player.getState() == PlayerState::Spin &&
                prevState != PlayerState::Spin) {
                combat.spawnSpinHitbox(player);
            }

            player.update(gameDt);
            world.updateEnemies(player, gameDt, world.currentRoom().map);
            combat.steerNarutoProjectiles(gameDt, player, world.getEntities());
            world.getEntities().updateAll(gameDt);
            combat.update(gameDt, player, world.getEntities());
            interaction.update(gameDt, player, world.getEntities());

            DoorSide side;
            world.tryTransition(player, side);

            if (player.getStats().isDead()) {
                saveData.totalDeaths++;
                runTracker.submitScore(saveData);
                state = GameState::GameOver;
                AudioManager::instance().playGameOverMusic();
            }
            if (world.currentRoom().type == RoomType::Boss &&
                world.currentRoom().cleared) {
                runTracker.submitScore(saveData);
                state = GameState::Victory;
            }
        }

        playerRenderer.update(player, gameDt);
        entityRenderer.update(world.getEntities(), gameDt);
        camera.update(gameDt, player, world.currentRoom().map, window.getSize());

        window.clear(sf::Color(30, 30, 40));
        window.setView(camera.getView());
        tilemap.draw(window, world.currentRoom().map);
        entityRenderer.draw(window, player, world.getEntities());
        damageNumbers.update(dt);
        damageNumbers.draw(window);

        window.setView(window.getDefaultView());
        hud.draw(window, player);
        minimap.draw(window, world.getRooms(), world.getCurrentRoomId());

        if (state == GameState::Shop) {
            shopUI.draw(window, economy.getShop(), player);
            shopUI.handleInput(economy.getShop(), player);
        }
        if (state == GameState::Paused) screens.drawPause(window);
        if (state == GameState::GameOver) screens.drawGameOver(window);
        if (state == GameState::Victory) screens.drawVictory(window);

        window.display();
    }

    saveSystem.captureFromPlayer(player, saveData);
    saveSystem.save(saveData);
}
