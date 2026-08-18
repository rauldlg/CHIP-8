#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include "chp8.h"
#include <string.h>


void initialize_chip8(Chip8* chp, unsigned short* mem)
{
    memset(chp->V, 0, sizeof(char)*16);
    chp->I = 0;
    chp->DT = 0;
    chp->ST = 0;
    chp->PC = &mem[0x200];
    memset(chp->stack, 0, sizeof(short)*16);
    chp->SP = &chp->stack[16];
    chp->IR = 0;
    InitWindow(64*10, 32*10, "CHIP-8 Interpreter");
    memset(chp->screen, 0, sizeof(int)*64*32);
}

void print_chip8(Chip8* chp)
{
    printf("IR=0x%04x\nI=0x%04x\nPC=%p\n", chp->IR, chp->I,(void*)chp->PC);
    for(int i = 0; i <= 0xF; i++)
    {
        printf("V%X=0x%04x\n", i, chp->V[i]);
    }
}

void fetch(Chip8* chp)
{
    chp->IR = *chp->PC;
}

void decode(Chip8* chp)
{
    chp->IR = ((chp->IR & 0x00FF) << 8) | ((chp->IR & 0xFF00) >> 8);
}


void execute(Chip8* chp)
{
    unsigned char x = 0;
    switch((chp->IR & 0xF000) >> 12)
    {
        case 0x3:
            x = ((chp->IR & 0x0F00) >> 8);
            if(chp->V[x] == (chp->IR & 0x00FF))
            {
                chp->PC+=2;
                break;
            }
            chp->PC++;
            break;
        case 0xA:
            chp->I = chp->IR & 0x0FFF;
            chp->PC++;
            break;
        case 0xC:
             x = ((chp->IR & 0x0F00) >> 8);
            chp->V[x] = (random() % 256) & (chp->IR & 0x00FF);
            chp->PC++;
            break;
        case 0xD:
            // get the Vx register
            x = (chp->IR & 0x0F00) >> 8;
            // get the Vy register
            unsigned char y = (chp->IR & 0x00F0) >> 4;

            // get the x and y position at Vx and Vy
            x = chp->V[x];
            y = chp->V[y];


            BeginDrawing();
                ClearBackground(BLACK);
            EndDrawing();
            // fetch I location
            chp->IR = chp->I;
            // decode Instruction to draw
            chp->IR = ((chp->IR & 0x00FF) << 8) | ((chp->IR & 0xFF00) >> 8);


    }
}



