cmake_minimum_required(VERSION 3.10)

# Nombre del proyecto
project(ZeldaASCII)

# Estándar de C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Buscar pthreads
find_package(Threads REQUIRED)

# Incluir headers
include_directories(
    ${PROJECT_SOURCE_DIR}/src
)

# Archivos fuente
file(GLOB_RECURSE SOURCES
    src/*.cpp
)

# Crear ejecutable
add_executable(ZeldaASCII ${SOURCES})

# Link con pthread
target_link_libraries(ZeldaASCII Threads::Threads)