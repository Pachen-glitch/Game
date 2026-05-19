cmake_minimum_required(VERSION 3.10)

project(ZeldaASCII)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Buscar pthreads
find_package(Threads REQUIRED)

# Incluir headers
include_directories(
    ${PROJECT_SOURCE_DIR}/src
)

# Buscar todos los .cpp
file(GLOB_RECURSE SOURCES
    src/*.cpp
)

# Crear ejecutable
add_executable(ZeldaASCII ${SOURCES})

# Link threads
target_link_libraries(ZeldaASCII Threads::Threads)