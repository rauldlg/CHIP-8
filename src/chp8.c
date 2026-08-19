//#include <raylib.h>
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
    chp->mem = mem;
    chp->PC = &mem[0x100];
    memset(chp->stack, 0, sizeof(short)*16);
    chp->SP = &chp->stack[16];
    chp->IR = 0;
    memset(chp->display, 0, sizeof(int)*64*32);
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
    //printf("IR: %#06x\nPC: %p\nI: %#06x\n", chp->IR, chp->PC, chp->I);
    //print_chip8(chp);
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
            unsigned char display_byte = 0;
           for(int i = 0; i <= (chp->IR & 0x000F); i++)
           {
                for(int j = 0; j <= 8; j++)
                {
                    display_byte = ((char*)chp->mem)[chp->I-1];
                    // isn't test yet but i guess i have to move on
                    chp->display[x+j][y+i] = ((display_byte & (0x80>>j)) >> (7-j));
                }
           }
            chp->PC++;
           print_display(chp);
           break;
    }
}


void print_display(Chip8* chp)
{
    for(int y = 0; y < 32; y++)
    {
        for(int x = 0; x < 64; x++)
        {
            printf("[%d]", chp->display[x][y]);
        }
        putchar('\n');
    }
}
