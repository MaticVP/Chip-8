#include <iostream>

#include "Include/main.h"

int main(int argv, char** args) {
    std::cout << "Hello, welcome to chip-8 emulator!" << std::endl;
    Emulator emu;

    char path[] = R"(D:\delo\Chip-8-emulator\ROMS\games\Tetris [Fran Dachille, 1991].ch8)";

    emu.load_rom(path);

    emu.run();


    return 0;
}