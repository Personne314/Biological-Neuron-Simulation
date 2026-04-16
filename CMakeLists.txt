cmake_minimum_required(VERSION 3.20)

project(bio_sim
    LANGUAGES CXX
)

# --- Standard C++ ---
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- Flags équivalents à ton Makefile ---
add_compile_options(
    -Wall
    -Wextra
    -O3
)

# --- Dépendances ---
find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(GLEW REQUIRED)

# --- Sources ---
file(GLOB_RECURSE SOURCES
    "src/*.cpp"
)

# --- Exécutable ---
add_executable(bio_sim ${SOURCES})

# --- Includes (équivalent -I. -I../../) ---
target_include_directories(bio_sim PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
    ${SDL2_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
)

# --- Link ---
target_link_libraries(bio_sim PRIVATE
    SDL2::SDL2
    GLEW::GLEW
    OpenGL::GL
)