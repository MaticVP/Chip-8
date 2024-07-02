//
// Created by matic on 30/06/2024.
//

#ifndef CHIP_8_EMULATOR_CPU_H
#define CHIP_8_EMULATOR_CPU_H
#include "Config.h"

struct CPU {
    unsigned char registers[16];
    unsigned char stackPointer=0;
    unsigned short PC = 512;
    unsigned short I = 0;
    unsigned short stack[STACK_SIZE];
    unsigned char memory[MEMORY_SIZE];
};


#endif //CHIP_8_EMULATOR_CPU_H
