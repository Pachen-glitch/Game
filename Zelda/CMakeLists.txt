cmake_minimum_required(VERSION 3.16)

project(TheLegendOfPachen)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

# ========================================
# SFML
# ========================================

FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.1
)

FetchContent_MakeAvailable(SFML)

# ========================================
# SOURCES
# ========================================

file(GLOB_RECURSE SOURCES
    src/*.cpp
)

# ========================================
# EXECUTABLE
# ========================================

add_executable(
    ZeldaASCII
    ${SOURCES}
)

# ========================================
# LINK
# ========================================

target_link_libraries(
    ZeldaASCII
    sfml-graphics
    sfml-window
    sfml-system
)