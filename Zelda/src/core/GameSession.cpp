#include "GameSession.h"

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

#include <SFML/Graphics.hpp>
#include <SFML/Window/Keyboard.hpp>

#include <filesystem>

#ifdef _WIN32
#include <windows.h>
#endif

namespace {

void setWorkingDirectoryToExecutableFolder() {
    std::error_code ec;
#ifdef _WIN32
    wchar_t buffer[MAX_PATH];
    DWORD len = GetModuleFileNameW(nullptr, buffer, MAX_PATH);
    if (len > 0 && len < MAX_PATH) {
        std::filesystem::current_path(
            std::filesystem::path(buffer).parent_path(), ec
        );
    }
#else
    (void)ec;
#endif
}

} // namespace

void GameSession::setupEvents() {
    EventBus& bus = EventBus::instance();

    bus.subscribe("player_attack", []() {
        AudioManager::instance().playSound("attack");
    });
    bus.subscribe("rupee_pickup", []() {
        AudioManager::instance().playSound("pickup");
    });
    bus.subscribe("enemy_died", []() {
        AudioManager::instance().playSound("enemy_death");
    });
    bus.subscribe("player_damaged", []() {
        AudioManager::instance().playSound("hurt");
    });
}

void GameSession::run() {
    // Carpeta del .exe (ahí están assets/ y las DLL de SFML).
    // Antes hacía chdir("Debug") y rompía si ya estabas en build\Debug.
    setWorkingDirectoryToExecutableFolder();

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
        state = GameState::Shop;
        economy.openShop(player, world.getCurrentRoomId());
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

        if (state == GameState::Playing) {
            PlayerState prevState = player.getState();
            movement.update(dt, player, world.currentRoom().map);

            if (player.getState() == PlayerState::Attack &&
                prevState != PlayerState::Attack) {
                combat.spawnSwordHitbox(player);
            }
            if (player.getState() == PlayerState::Spin &&
                prevState != PlayerState::Spin) {
                combat.spawnSpinHitbox(player);
            }

            player.update(dt);
            world.updateEnemies(player, dt);
            world.getEntities().updateAll(dt);
            combat.update(dt, player, world.getEntities());
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

        playerRenderer.update(player, dt);
        camera.update(dt, player, window.getSize());

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
