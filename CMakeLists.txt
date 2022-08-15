cmake_minimum_required(VERSION 3.22)
project(Solitaire)

set(CMAKE_CXX_STANDARD 14)

add_executable(Solitaire main.cpp)

find_library(SDL2 SDL2 lib)
find_library(SDL2_MAIN SDL2main lib)
find_library(SDL2_IMAGE SDL2_image lib)
target_include_directories(Solitaire PRIVATE include/SDL2)
target_link_libraries(Solitaire PRIVATE ${SDL2_MAIN} ${SDL2} ${SDL2_IMAGE})

target_compile_definitions(${PROJECT_NAME} PUBLIC ASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/res/") # Set the asset path macro to the absolute path on the dev machine
#target_compile_definitions(${PROJECT_NAME} PUBLIC ASSETS_PATH="./assets") # Set the asset path macro in release mode to a relative path that assumes the assets folder is in the same directory as the game executable
