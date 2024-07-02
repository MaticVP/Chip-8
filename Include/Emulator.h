//
// Created by matic on 30/06/2024.
//

#ifndef CHIP_8_EMULATOR_EMULATOR_H
#define CHIP_8_EMULATOR_EMULATOR_H

#include "CPU.h"
#include "Config.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <SDL.h>
#include <chrono>

struct SDL_app {
    SDL_Renderer *renderer;
    SDL_Window *window;
};

class Emulator {
public:
    CPU cpu;
    SDL_app sdlApp;
    bool stop;
    int cpu_frequency = 540;
    const int timer_frequency  = 60;
    unsigned short sound_timer;
    unsigned short delay_timer;
    Emulator();
    unsigned char display[32*64];
    unsigned short keyboard;
    void load_rom(const char* path);
    void execute_instruction();
    void initSDL();
    void draw();
    void subSwitch8(unsigned char second_nibble,unsigned char third_nibble
                    ,unsigned char forth_nibble);
    void subSwitch15(unsigned char second_nibble, unsigned short second_byte);

    void run();
};


#endif //CHIP_8_EMULATOR_EMULATOR_H