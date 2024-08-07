//
// Created by matic on 30/06/2024.
//

#include "Include/Emulator.h"

void Emulator::load_rom(const char* path) {
    std::ifstream rom(path, std::ios::binary);
    if(!rom.fail()) {
        std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(rom), {});
        std::memcpy(&this->cpu.memory[this->cpu.PC],buffer.data(),buffer.size() * sizeof(unsigned char));
    }
    else{
        std::cout << "File reading failed :(" << std::endl;
    }
}

void Emulator::subSwitch15(unsigned char second_nibble, unsigned short second_byte) {
    switch (second_byte) {
        case 0x07:{
            this->cpu.registers[second_nibble] = this->delay_timer;
            break;
        }
        case 0x0A:{
            unsigned short key_state = this->keyboard & 0xFFFF;
            if(!key_state) {
                this->cpu.PC -= 2;
            }
            else{
                for (int i = 0; i < 16; ++i) {
                    if(this->keyboard & 1 << i){
                        this->cpu.registers[second_nibble] = i;
                        break;
                    }
                }
            }
            this->keyboard = 0;
            break;
        }
        case 0x15:{
            this->delay_timer = this->cpu.registers[second_nibble];
            break;
        }
        case 0x18:{
            this->sound_timer = this->cpu.registers[second_nibble];
            break;
        }
        case 0x1E:{
            this->cpu.I += this->cpu.registers[second_nibble];
            break;
        }
        case 0x29:{
            this->cpu.I = 5*this->cpu.registers[second_nibble];
            break;
        }
        case 0x33:{
            unsigned char value =  cpu.registers[second_nibble];
            cpu.memory[cpu.I] = (value/100) % 10;
            cpu.memory[cpu.I+1] = (value/10) % 10;
            cpu.memory[cpu.I+2] = (value%10);
            break;
        }
        case 0x55:{
            for(int i=0;i<second_nibble+1;i++){
                cpu.memory[this->cpu.I+i] = cpu.registers[i];
            }
            break;
        }
        case 0x65:{
            for(int i=0;i<second_nibble+1;i++){
                cpu.registers[i] = cpu.memory[this->cpu.I+i];
            }
            break;
        }
    }
}

void Emulator::subSwitch8(unsigned char second_nibble, unsigned char third_nibble,
                          unsigned char forth_nibble) {
    switch (forth_nibble) {
        case 0:{
            this->cpu.registers[second_nibble] = this->cpu.registers[third_nibble];
            break;
        }
        case 1:{
            this->cpu.registers[second_nibble] |= this->cpu.registers[third_nibble];
            break;
        }
        case 2:{
            this->cpu.registers[second_nibble] &= this->cpu.registers[third_nibble];
            break;
        }
        case 3:{
            this->cpu.registers[second_nibble] ^= this->cpu.registers[third_nibble];
            break;
        }
        case 4:{
            unsigned char a = this->cpu.registers[second_nibble];
            unsigned char b = this->cpu.registers[third_nibble];

            if(a+b<a)
            {
                this->cpu.registers[15] = 1;
            }
            else{
                this->cpu.registers[15] = 0;
            }

            this->cpu.registers[second_nibble] += this->cpu.registers[third_nibble];
            break;
        }
        case 5:{
            unsigned char a = this->cpu.registers[second_nibble];
            unsigned char b = this->cpu.registers[third_nibble];

            if(a-b>a)
            {
                this->cpu.registers[15] = 1;
            }
            else{
                this->cpu.registers[15] = 0;
            }

            this->cpu.registers[second_nibble] -= this->cpu.registers[third_nibble];
            break;
        }
        case 6:{
            this->cpu.registers[15] =  this->cpu.registers[second_nibble] & 1;
            this->cpu.registers[second_nibble] >>=1;
            break;
        }
        case 7:{
            unsigned char a = this->cpu.registers[second_nibble];
            unsigned char b = this->cpu.registers[third_nibble];

            if(a-b>b)
            {
                this->cpu.registers[15] = 1;
            }
            else{
                this->cpu.registers[15] = 0;
            }

            this->cpu.registers[second_nibble] = this->cpu.registers[third_nibble] - this->cpu.registers[second_nibble];
            break;
        }
        case 14:{
            this->cpu.registers[15] =  this->cpu.registers[second_nibble] >> 7;
            this->cpu.registers[second_nibble] <<=1;
            break;
        }
        default:{
            std::cout << "Unknown variation of 8XY instruction format" << std::endl;
            break;
        }
    }

}

void Emulator::execute_instruction() {

    unsigned char first_nibble = this->cpu.memory[this->cpu.PC] >> 4;
    unsigned char second_nibble = this->cpu.memory[this->cpu.PC] & 0xF;
    unsigned char second_byte = this->cpu.memory[this->cpu.PC+1];
    unsigned char third_nibble = second_byte >> 4;
    unsigned char forth_nibble = second_byte & 0xF;

    switch (first_nibble) {
        case 0: {
            if (second_nibble != 0) {
                cpu.stack[cpu.stackPointer] = cpu.PC;
                cpu.stackPointer++;
                cpu.PC = (unsigned short) second_nibble << 8 | second_byte;

            } else if (second_byte == 224) {
                SDL_SetRenderDrawColor(this->sdlApp.renderer,0,0,0,0);
                SDL_RenderClear(this->sdlApp.renderer);
                SDL_RenderPresent(this->sdlApp.renderer);
                std::memset(this->display,0,sizeof(unsigned char)*64*32);
                this->cpu.PC += 2;
            } else {
                cpu.stackPointer--;
                cpu.PC = cpu.stack[cpu.stackPointer];
                this->cpu.PC += 2;
            }
            break;
        }
        case 1:{
            cpu.PC = (unsigned short) second_nibble << 8 | second_byte;
            break;
        }
        case 2:{
            cpu.stack[cpu.stackPointer] = cpu.PC;
            cpu.stackPointer++;
            cpu.PC = (unsigned short) second_nibble << 8 | second_byte;
            break;
        }
        case 3: {
            if(this->cpu.registers[second_nibble]==second_byte){
                this->cpu.PC+=4;
            }
            else{
                this->cpu.PC+=2;
            }
            break;
        }
        case 4: {
            if(this->cpu.registers[second_nibble]!=second_byte){
                this->cpu.PC+=4;
            }
            else{
                this->cpu.PC+=2;
            }
            break;
        }
        case 5: {
            if(this->cpu.registers[second_nibble]==this->cpu.registers[third_nibble]){
                this->cpu.PC+=4;
            }
            else{
                this->cpu.PC+=2;
            }
            break;
        }
        case 6: {
            this->cpu.registers[second_nibble] = second_byte;
            this->cpu.PC += 2;
            break;
        }
        case 7: {
            this->cpu.registers[second_nibble] += second_byte;
            this->cpu.PC += 2;
            break;
        }
        case 8: {
            this->subSwitch8(second_nibble,third_nibble,forth_nibble);
            this->cpu.PC += 2;
            break;
        }
        case 9: {
            if(this->cpu.registers[second_nibble]!=this->cpu.registers[third_nibble]){
                this->cpu.PC+=4;
            }
            else{
                this->cpu.PC+=2;
            }
            break;
        }
        case 10: {
            this->cpu.I = (unsigned short) second_nibble << 8 | second_byte;
            this->cpu.PC += 2;
            break;
        }
        case 11: {
            this->cpu.PC = this->cpu.registers[0] + ((unsigned short) second_nibble << 8 | second_byte);
            break;
        }
        case 12: {
            short randomNumber = std::rand() % 255;
            this->cpu.registers[second_nibble] = randomNumber & second_byte;
            this->cpu.PC += 2;
            break;
        }
        case 13: {
            unsigned char X = this->cpu.registers[second_nibble] % 64;
            unsigned char Y = this->cpu.registers[third_nibble] % 32;
            unsigned char const N = forth_nibble;
            this->cpu.registers[15] = 0;

            for (int n = 0; n < N; n++) {
                unsigned char sprite_byte = this->cpu.memory[cpu.I + n];
                for (int i = X; i < X+9; i++) {
                    unsigned char pixel_value = this->display[i + 64 * Y];
                    unsigned char sprite_bit = ((sprite_byte & 1 << ((X+8)-i)) >> ((X+8)-i));
                    if( pixel_value==1 && sprite_bit!=0)
                    {
                        this->display[i + 64 * Y] = 0;
                        this->cpu.registers[15] = 1;
                    }
                    else if(pixel_value==0 && sprite_bit!=0)
                    {
                        this->display[i + 64 * Y] = 1;
                    }
                }
                Y++;
            }
            this->cpu.PC += 2;
            break;
        }
        case 14:{
            if (second_byte==0x9E){
                unsigned  short key = this->keyboard & 1 << this->cpu.registers[second_nibble];
                if(key) {
                    this->cpu.PC += 4;
                }
                else{
                    this->cpu.PC += 2;
                }
                //this->keyboard = 0;
                break;
            }
            else{
                unsigned  short key = this->keyboard & 1 << this->cpu.registers[second_nibble];
                if(!key) {
                    this->cpu.PC += 4;
                }
                else{
                    this->cpu.PC += 2;
                }
                //this->keyboard = 0;
                break;
            }
        }
        case 15:{
            subSwitch15(second_nibble, second_byte);
            this->cpu.PC+=2;
            break;
        }
        default: {
            std::cout << "Unrecognized instruction nibble " << first_nibble << std::endl;
            break;
        }
    }
}

void Emulator::initSDL() {
    int rendererFlags, windowFlags;

    rendererFlags = SDL_RENDERER_ACCELERATED;

    windowFlags = 0;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("Couldn't initialize SDL: %s\n", SDL_GetError());
        exit(1);
    }

    this->sdlApp.window = SDL_CreateWindow("Chip-8 emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, windowFlags);

    if (!this->sdlApp.window)
    {
        printf("Failed to open %d x %d window: %s\n", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_GetError());
        exit(1);
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

    this->sdlApp.renderer = SDL_CreateRenderer(this->sdlApp.window, -1, rendererFlags);

    if (!this->sdlApp.renderer)
    {
        printf("Failed to create renderer: %s\n", SDL_GetError());
        exit(1);
    }

    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "Could not initialize SDL: %s\n", SDL_GetError());
        exit(1);
    }
}

Emulator::Emulator() {
    memset(this->display,0,sizeof(unsigned  char)*64*32);
    memset(this->cpu.memory,0,sizeof(unsigned  char)*4096);

    unsigned const char font_array[] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80
    };

    std::memcpy(this->cpu.memory,font_array,sizeof(unsigned  char)*80);


    this->stop = false;
    this->sound_timer = 0;
    this->delay_timer = 0;

    this->keyboard = 0;

    initSDL();
}

void Emulator::draw() {

    SDL_RenderClear(this->sdlApp.renderer);
    int scaleX = WINDOW_WIDTH / 64;
    int scaleY = WINDOW_HEIGHT / 32;
    SDL_SetRenderDrawColor(this->sdlApp.renderer,255,255,255,255);
    for (int y = 0; y < 32; ++y) {
        for (int x = 0; x < 64; ++x) {
            if (this->display[x + 64 * y]) {
                SDL_Rect rect;
                rect.x = (int)(x * scaleX);
                rect.y = (int)(y * scaleY);
                rect.w = (int)scaleX;
                rect.h = (int)scaleY;
                SDL_RenderFillRect(this->sdlApp.renderer, &rect);
            }
        }
    }
    SDL_SetRenderDrawColor(this->sdlApp.renderer,0,0,0,0);
    SDL_RenderPresent(this->sdlApp.renderer);
}

void Emulator::run() {

    auto cpuUpdate = std::chrono::high_resolution_clock ::now();
    auto timerUpdate = cpuUpdate;

    while(!this->stop) {

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                this->stop = true;
            }
            else if (event.type == SDL_KEYDOWN) {
                SDL_Keycode keycode = event.key.keysym.sym;
                const char* key = SDL_GetKeyName(keycode);
                switch (*key) {
                    case '1':
                        keyboard |= 1 << 1;
                        break;
                    case '2':
                        keyboard |= 1 << 2;
                        break;
                    case '3':
                        keyboard |= 1 << 3;
                        break;
                    case '4':
                        keyboard |= 1 << 0xc;
                        break;
                    case 'Q':
                        keyboard |= 1 << 4;
                        break;
                    case 'W':
                        keyboard |= 1 << 5;
                        break;
                    case 'E':
                        keyboard |= 1 << 6;
                        break;
                    case 'R':
                        keyboard |= 1 << 0xd;
                        break;
                    case 'A':
                        keyboard |= 1 << 7;
                        break;
                    case 'S':
                        keyboard |= 1 << 8;
                        break;
                    case 'D':
                        keyboard |= 1 << 9;
                        break;
                    case 'F':
                        keyboard |= 1 << 0xe;
                        break;
                    case 'Y':
                        keyboard |= 1 << 0xa;
                        break;
                    case 'X':
                        keyboard |= 1 << 0;
                        break;
                    case 'C':
                        keyboard |= 1 << 0xb;
                        break;
                    case 'V':
                        keyboard |= 1 << 0xf;
                        break;
                }
            }
        }

        auto currentTime = std::chrono::high_resolution_clock ::now();

        std::chrono::duration<double, std::milli> cpuDelata = currentTime - cpuUpdate;
        std::chrono::duration<double, std::milli> timerDelata = currentTime - timerUpdate;

        double cycleCPU = (double)1 / this->cpu_frequency;
        double cycleTimer = (double)1 / this->timer_frequency;

        if((cpuDelata.count()/1000)>cycleCPU)
        {
            cpuUpdate = currentTime;
            this->execute_instruction();
            keyboard = 0;
        }

        if((timerDelata.count()/1000)>cycleTimer)
        {
            timerUpdate = currentTime;
            this->draw();
            if(this->sound_timer>0){
                Beep(523,950);
                this->sound_timer--;
            }
            else{
                SDL_PauseAudio(1);
            }

            if(this->delay_timer>0){
                delay_timer--;
            }

        }
    }
}


