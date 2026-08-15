#include <stdio.h>
#include "chp8.h"
#include <string.h>
void initialize_chip8(Chip8* chp, unsigned short* mem)
{
    memset(chp->V, 0, sizeof(char)*16);
    chp->I = 0;
    chp->DT = 0;
    chp->ST = 0;
    chp->PC = mem;
    memset(chp->stack, 0, sizeof(short)*16);
    chp->SP = &chp->stack[16];
    chp->IR = 0;
}

unsigned short fetch(Chip8* chp)
{
    chp->IR = *chp->PC;
    chp->IR = ((chp->IR & 0x00FF) << 8) | ((chp->IR & 0xFF00) >> 8);
    //printf("fetch: %#06x\n", chp->IR);
    chp->PC++;
    //return OPCODE to decode
    return chp->IR;
}

void decode(Chip8* chp, unsigned short* opcode)
{
    switch((*opcode & 0xF000) >> 12)
    {
        case 0xA:
            //printf("\nopcode: Annn\n");
            //printf("\nI Register: 0x%04x", chp->I);
            chp->I = *opcode & 0x0FFF;
            //printf("\nI Register after: 0x%04x\n", chp->I);
            break;
    }
}
