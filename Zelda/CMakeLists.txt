cmake_minimum_required(VERSION 3.16)

project(TheLegendOfPachen)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.1
)

FetchContent_MakeAvailable(SFML)

include_directories(src)

set(SOURCES
    src/main.cpp

    # Core
    src/core/Game.cpp
    src/core/GameSession.cpp
    src/core/CombatFeel.cpp

    # Interaction
    src/interaction/EventBus.cpp
    src/interaction/InteractionSystem.cpp

    # Map
    src/map/Map.cpp

    # Entity
    src/entity/base/Entity.cpp
    src/entity/base/EntityManager.cpp

    src/entity/player/Player.cpp
    src/entity/player/PlayerStats.cpp

    src/entity/enemy/Enemy.cpp
    src/entity/enemy/EnemyAI.cpp
    src/entity/enemy/SlimeEnemy.cpp
    src/entity/enemy/BatEnemy.cpp
    src/entity/enemy/SkeletonEnemy.cpp
    src/entity/enemy/SummonerEnemy.cpp

    src/entity/items/Coin.cpp
    src/entity/items/Key.cpp
    src/entity/items/Heart.cpp
    src/entity/items/Door.cpp
    src/entity/items/Chest.cpp
    src/entity/npc/Shopkeeper.cpp

    # Movement
    src/movement/PlayerMovement.cpp
    src/movement/MovementSystem.cpp
    src/movement/CollisionSystem.cpp
    src/movement/EnemyMovement.cpp

    # Combat
    src/combat/CombatSystem.cpp

    # Animation
    src/animation/Animation.cpp
    src/animation/AnimationManager.cpp

    # Render
    src/render/TextureCache.cpp
    src/render/PlayerRenderer.cpp
    src/render/SkeletonRenderer.cpp
    src/render/Camera.cpp
    src/render/TilemapRenderer.cpp
    src/render/EntityRenderer.cpp
    src/render/EnemyAnimator.cpp

    # Procedural
    src/procedural/Room.cpp
    src/procedural/DungeonGenerator.cpp
    src/procedural/World.cpp

    # Economy
    src/economy/Shop.cpp
    src/economy/EconomySystem.cpp

    # UI
    src/ui/Menu.cpp
    src/ui/menu/MenuScreenBase.cpp
    src/ui/menu/MainMenu.cpp
    src/ui/menu/ControlsScreen.cpp
    src/ui/menu/InstructionsScreen.cpp
    src/ui/menu/StatisticsScreen.cpp
    src/ui/HUD.cpp
    src/ui/Minimap.cpp
    src/ui/ShopUI.cpp
    src/ui/DamageNumbers.cpp
    src/ui/GameScreens.cpp

    # Audio
    src/audio/AudioManager.cpp

    # Save
    src/save/SaveSystem.cpp

    # Utils
    src/utils/FontLoader.cpp
    src/utils/AssetPaths.cpp

    # Threads
    src/threads/JobQueue.cpp
    src/threads/ThreadPool.cpp
    src/threads/AsyncSemaphore.cpp
)

add_executable(ZeldaASCII ${SOURCES})

target_link_libraries(ZeldaASCII
    sfml-graphics
    sfml-window
    sfml-system
    sfml-audio
)

if(WIN32)

    add_custom_command(
        TARGET ZeldaASCII POST_BUILD

        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_RUNTIME_DLLS:ZeldaASCII>
        $<TARGET_FILE_DIR:ZeldaASCII>

        COMMAND_EXPAND_LISTS
    )
endif()

add_custom_command(
    TARGET ZeldaASCII POST_BUILD

    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    $<TARGET_FILE_DIR:ZeldaASCII>/assets
)