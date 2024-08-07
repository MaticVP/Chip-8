#include <iostream>

#include "Include/main.h"

int main(int argv, char** args) {
    std::cout << "Hello, welcome to chip-8 emulator!" << std::endl;
    Emulator emu;


    emu.load_rom(args[1]);

    emu.run();


    return 0;
}