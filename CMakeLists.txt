cmake_minimum_required(VERSION 3.16)
project(chess_sfml LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

add_executable(chess_game
    src/main.cpp
    src/Game.cpp
    src/Board.cpp
    src/Piece.cpp
)

target_include_directories(chess_game PRIVATE include)
target_link_libraries(chess_game PRIVATE sfml-graphics sfml-window sfml-system)
