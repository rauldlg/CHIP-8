#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "chp8.h"
#include <string.h>


void initialize_chip8(Chip8* chp, unsigned char* mem)
{
    memset(chp->V, 0, sizeof(char)*16);
    chp->I = 0;
    chp->DT = 0;
    chp->ST = 0;
    chp->mem = mem;
    chp->PC = (unsigned short*)&mem[0x200];
    memset(chp->stack, 0, sizeof(short)*16);
    chp->SP = 15;
    chp->IR = 0;
    memset(chp->display, 0, sizeof(int)*64*32);
}

void print_chip8(Chip8* chp)
{
    printf("PC=%p\nIR=0x%04x\nI=0x%04x\n", (void*)chp->PC, chp->IR, chp->I);
    for(int i = 0; i <= 0x4; i++)
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
    print_chip8(chp);
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
            chp->V[x] =  (chp->IR & 0x00FF) & (rand() % 256);
            chp->PC++;
            break;
        case 0xD:
            // get the Vx register
            x = (chp->IR & 0x0F00) >> 8;
            // get the Vy register
            unsigned char y = (chp->IR & 0x00F0) >> 4;

            // get the x and y position at Vx and Vy
            x = chp->V[x] % 63;
            y = chp->V[y] % 31;
            unsigned char display_byte = 0;
           for(int i = 0; i < (chp->IR & 0x000F); i++)
           {
                for(int j = 0; j <= 8; j++)
                {
                    display_byte = ((char*)chp->mem)[chp->I+i];
                    // isn't test yet but i guess i have to move on
                    unsigned char collide = chp->display[x+j][y+i];
                    chp->display[x+j][y+i] ^= ((display_byte & (0x80>>j)) >> (7-j));
                    chp->V[0xF] = 0;
                    if((collide == 1) && (chp->display[x+j][y+i] == 0))
                    {
                        chp->V[0xF] = 1;
                    }
                }
           }
            printf("x: %d\ny: %d\n", x, y);
            chp->PC++;
           break;
        case 0x7:
           x = (chp->IR & 0x0F00) >> 8;
           chp->V[x] += (chp->IR & 0x00FF);
           chp->PC++;
           break;
        case 0x1:
           unsigned short loc = chp->IR & 0x0FFF;
           chp->PC = (unsigned short*)&chp->mem[loc];
           break;
        case 0x6:
            x = (chp->IR & 0x0F00) >> 8;
            chp->V[x] = (chp->IR & 0x00FF);
            chp->PC++;
            break;
        case 0x8:
            if((chp->IR & 0x000F) == 0)
            {
                x = (chp->IR & 0x0F00) >> 8;
                y = (chp->IR & 0x00F0) >> 4;
                chp->V[x] = chp->V[y];
                chp->PC++;
            }
            break;
        case 0x2:
            chp->stack[chp->SP] = chp->PC;
            chp->PC = (unsigned short*)&chp->mem[chp->IR & 0x0FFF];
            chp->SP--;
            break;
        case 0x0:
            if((chp->IR & 0x0F00) == 0)
            {
                if( (chp->IR & 0x000F) == 0xE)
                {
                    chp->PC = chp->stack[chp->SP];
                    chp->SP++;
                }
            }
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
