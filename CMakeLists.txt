cmake_minimum_required(VERSION 3.10)
project(TapRush)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories("C:/Users/neel969/Desktop/DEVELOPMENT/TapRush/raylib/raylib-6.0_win64_mingw-w64")

link_directories("C:/Users/neel969/Desktop/DEVELOPMENT/TapRush/raylib/raylib-6.0_win64_mingw-w64/lib")

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(TapRush src/main.cpp src/Game.cpp src/Button.cpp)

target_link_libraries(TapRush raylib)
target_link_libraries(TapRush winmm gdi32)