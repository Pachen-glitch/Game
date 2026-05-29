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
#include "../ui/DamageNumbers.h"
#include "../ui/GameScreens.h"
#include "../ui/HUD.h"
#include "../ui/Minimap.h"
#include "../ui/ShopUI.h"
#include "../utils/AssetPaths.h"

#include <SFML/Graphics.hpp>
#include <SFML/Window/Keyboard.hpp>

void GameSession::setupEvents() {
    EventBus& bus = EventBus::instance();

    bus.subscribe("player_attack", []() {
        AudioManager::instance().playSound("attack");
    });
    bus.subscribe("rupee_pickup", []() {
        AudioManager::instance().playSound("pickup");
    });
    bus.subscribe("enemy_hit", []() {
        CombatFeel::instance().triggerHitPause(Constants::HIT_PAUSE_DURATION);
    });
    bus.subscribe("enemy_died", []() {
        AudioManager::instance().playSound("enemy_death");
    });
    bus.subscribe("player_damaged", []() {
        AudioManager::instance().playSound("hurt");
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

    setupEvents();
    AudioManager::instance().load();

    SaveData saveData;
    SaveSystem saveSystem;
    saveSystem.load(saveData);

    World world;
    world.newRun();

    Player player(world.currentRoom().getPlayerSpawn());
    saveSystem.applyToPlayer(player, saveData);

    MovementSystem movement;
    CombatSystem combat;
    InteractionSystem interaction;
    EconomySystem economy;
    economy.openShop(player, 42);

    PlayerRenderer playerRenderer;
    TilemapRenderer tilemap;
    EntityRenderer entityRenderer;
    Camera camera;
    HUD hud;
    Minimap minimap;
    ShopUI shopUI;
    GameScreens screens;
    DamageNumbers damageNumbers;

    GameState state = GameState::Playing;
    sf::Clock clock;

    EventBus::instance().subscribe("open_shop", [&]() {

        if (state == GameState::Shop)
            return;

        state = GameState::Shop;

        economy.openShop(
            player,
            world.getCurrentRoomId()
        );
    });

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();

        sf::Event ev;
        while (window.pollEvent(ev)) {
            if (ev.type == sf::Event::Closed) window.close();
            if (ev.type == sf::Event::KeyPressed) {
                if (ev.key.code == sf::Keyboard::Escape) {
                    if (state == GameState::Shop) {
                        state = GameState::Playing;
                    } else {
                        state = (state == GameState::Paused)
                            ? GameState::Playing : GameState::Paused;
                    }
                }
                if (ev.key.code == sf::Keyboard::E) {
                    interaction.tryInteract(player, world.getEntities());
                }
                if (ev.key.code == sf::Keyboard::R &&
                    state == GameState::GameOver) {
                    world.newRun();
                    player = Player(world.currentRoom().getPlayerSpawn());
                    saveSystem.applyToPlayer(player, saveData);
                    state = GameState::Playing;
                }
            }
        }

        CombatFeel::instance().tick(dt);
        float gameDt = dt * CombatFeel::instance().getTimeScale();

        if (state == GameState::Playing) {
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
            world.getEntities().updateAll(gameDt);
            combat.update(gameDt, player, world.getEntities());
            interaction.update(player, world.getEntities());

            DoorSide side;
            world.tryTransition(player, side);

            if (player.getStats().isDead()) {
                state = GameState::GameOver;
            }
            if (world.currentRoom().type == RoomType::Boss &&
                world.currentRoom().cleared) {
                state = GameState::Victory;
            }
        }

        playerRenderer.update(player, gameDt);
        entityRenderer.update(world.getEntities(), gameDt);
        camera.update(gameDt, player, window.getSize());

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
