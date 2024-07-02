cmake_minimum_required(VERSION 3.26)
project(Chip_8_emulator)

set(CMAKE_CXX_STANDARD 23)

set(SDL2_DIR "./Libs/SDL2-2.30.4/cmake")
find_package(SDL2 REQUIRED)

add_executable(Chip_8_emulator main.cpp Emulator.cpp)
target_link_libraries(Chip_8_emulator ${SDL2_LIBRARIES})
include_directories(Chip_8_emulator ${SDL2_INCLUDE_DIRS})
