cmake_minimum_required(VERSION 3.16)

# ========================================
# PROJECT
# ========================================

project(TheLegendOfPachen)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ========================================
# SFML
# ========================================

include(FetchContent)

FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.1
)

FetchContent_MakeAvailable(SFML)

# ========================================
# INCLUDE DIRECTORIES
# ========================================

include_directories(
    src
)

# ========================================
# SOURCE FILES
# ========================================

set(SOURCES

    # ====================================
    # MAIN
    # ====================================

    src/main.cpp

    # ====================================
    # CORE
    # ====================================

    src/core/Game.cpp

    # ====================================
    # UI
    # ====================================

    src/ui/Menu.cpp

    # ====================================
    # MAP
    # ====================================

    src/map/Map.cpp

    # ====================================
    # ENTITY BASE
    # ====================================

    src/entity/base/Entity.cpp

    # ====================================
    # PLAYER
    # ====================================

    src/entity/player/Player.cpp

    # ====================================
    # ENEMIES
    # ====================================

    src/entity/enemy/Enemy.cpp

    # ====================================
    # ITEMS
    # ====================================

    src/entity/items/Coin.cpp
    src/entity/items/Key.cpp
    src/entity/items/Heart.cpp
    src/entity/items/Door.cpp

    # ====================================
    # INPUT
    # ====================================

    src/input/Input.cpp

    # ====================================
    # MOVEMENT
    # ====================================

    src/movement/PlayerMovement.cpp
)

# ========================================
# EXECUTABLE
# ========================================

add_executable(
    ZeldaASCII
    ${SOURCES}
)

# ========================================
# LINK LIBRARIES
# ========================================

target_link_libraries(
    ZeldaASCII

    sfml-graphics
    sfml-window
    sfml-system
)

# ========================================
# WINDOWS
# ========================================

if(WIN32)

    add_custom_command(
        TARGET ZeldaASCII POST_BUILD

        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_RUNTIME_DLLS:ZeldaASCII>
        $<TARGET_FILE_DIR:ZeldaASCII>

        COMMAND_EXPAND_LISTS
    )

endif()