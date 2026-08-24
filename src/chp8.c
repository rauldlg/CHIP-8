#include <unistd.h>
#include<raylib.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "chp8.h"
#include <string.h>

int keyboard(Chip8* chp);

void initialize_chip8(Chip8* chp, unsigned char* mem)
{
    memset(chp->V, 0, sizeof(char)*16);
    chp->I = 0;
    chp->DT = 0;

    chp->mem = mem;
    chp->PC = (unsigned short*)&mem[0x200];
    memset(chp->stack, 0, sizeof(short)*16);
    chp->SP = 15;
    chp->IR = 0;
    memset(chp->display, 0, sizeof(int)*64*32);

    unsigned char fonts[0x1FF] = 
    {
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
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };
    memcpy(chp->mem, fonts, 0x1ff); 
}

void print_chip8(Chip8* chp)
{
    printf("PC=%p\nIR=0x%04x\nI=0x%04x\nDT=0x%04x\nST=0x%04x\n", (void*)chp->PC, chp->IR, chp->I, chp->DT, chp->ST);
    for(int i = 0; i < 16; i++)
    {
        printf("V%X=0x%04x\n", i, chp->V[i]);
    }
    printf("SP=%d\n", chp->SP);
    for(int i = 15; i != 0; i--)
    {
        printf("Stack %d: %p\n", i, chp->stack[i]);
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
    getchar();

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
            x = chp->V[x] % 64;
            y = chp->V[y] % 32;
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
                break;
            }
            if((chp->IR & 0x000F) == 1)
            {
                x = (chp->IR & 0x0F00) >> 8;
                y = (chp->IR & 0x00F0) >> 4;
                chp->V[x] |= chp->V[y];
                chp->PC++;
                break;
            }
            if((chp->IR & 0x000F) == 2)
            {
                x = (chp->IR & 0x0F00) >> 8;
                y = (chp->IR & 0x00F0) >> 4;
                chp->V[x] &= chp->V[y];
                chp->PC++;
                break;
            }
            if((chp->IR & 0x000F) == 3)
            {
                x = (chp->IR & 0x0F00) >> 8;
                y = (chp->IR & 0x00F0) >> 4;
                chp->V[x] ^= chp->V[y];
                chp->PC++;
                break;
            }
            if((chp->IR & 0x000F) == 4)
            {
                x = (chp->IR & 0x0F00) >> 8;
                y = (chp->IR & 0x00F0) >> 4;
                chp->V[0xF] = 0;
                if((chp->V[x] + chp->V[y]) > 255)
                {
                    chp->V[0xF] = 1;
                }
                chp->V[x] += chp->V[y];
                chp->PC++;
                break;
            }
            if((chp->IR & 0x000F) == 5)
            {
                x = (chp->IR & 0x0F00) >> 8;
                y = (chp->IR & 0x00F0) >> 4;
                
                if(chp->V[x] > chp->V[y])
                {
                    chp->V[x] -= chp->V[y];
                    chp->V[0xF] = 1;
                    chp->PC++;
                    break;
                }
                chp->V[x] -= chp->V[y];
                chp->V[0x0F] = 0;
                chp->PC++;
            }
            if((chp->IR & 0x000F) == 6)
            {
                x = (chp->IR & 0x0F00) >> 8;
                chp->V[0xF] = 0;
                if(chp->V[x] & 1)
                {
                    chp->V[0xF] = 1;
                }
                chp->V[x] >>= 1;
                chp->PC++;
                break;
            }
            if((chp->IR & 0x000F) == 7)
            {
                x = (chp->IR & 0x0F00) >> 8;
                y = (chp->IR & 0x00F0) >> 4;
                
                if(chp->V[y] > chp->V[x])
                {
                    chp->V[x] = chp->V[y] - chp->V[x];
                    chp->V[0xF] = 1;
                    chp->PC++;
                    break;
                }
                chp->V[x] = chp->V[y] - chp->V[x];
                chp->V[0x0F] = 0;
                chp->PC++;
                break;
            }
            if((chp->IR & 0x000F) == 0xE)
            {
                x = (chp->IR & 0x0F00) >> 8;
                chp->V[0xF] = 0;
                if(chp->V[x] & 0x80)
                {
                    chp->V[0xF] = 1;
                }
                chp->V[x] <<= 1;
                chp->PC++;
                break;
            }
            break;
        case 0x9:
            x = (chp->IR & 0x0F00) >> 8;
            y = (chp->IR & 0x00F0) >> 4;
            if(chp->V[x] != chp->V[y])
            {
                chp->PC+=2;
                break;
            }
            chp->PC++;
            break;
        case 0x2:
            chp->stack[chp->SP] = chp->PC+1;
            chp->PC = (unsigned short*)&chp->mem[chp->IR & 0x0FFF];
            chp->SP--;
            break;
        case 0x0:
            if((chp->IR & 0x0F00) == 0)
            {
                if( (chp->IR & 0x000F) == 0xE)
                {
                    chp->PC = chp->stack[chp->SP+1];
                    chp->SP++;
                }
                if( (chp->IR & 0x000F) == 0x0)
                {
                    memset(chp->display, 0, sizeof(int)*64*32);
                    chp->PC++;
                }
            }
            break;
        case 0x5:
            x = (chp->IR & 0x0F00) >> 8;
            y = (chp->IR & 0x00F0) >> 4;
            if(chp->V[x] == chp->V[y])
            {
                chp->PC+=2;
                break;
            }
            chp->PC++;
            break;
        case 0x4:
            x = (chp->IR & 0x0F00) >> 8;
            if(chp->V[x]  != (chp->IR & 0x00FF))
            {
                chp->PC+=2;
                break;
            }
            chp->PC++;
            break;
        case 0xE:
            keyboard(chp);
            break;
        case 0xF:
            x = (chp->IR & 0x0F00) >> 8;
            if((chp->IR & 0x00FF) == 0x07)
            {
                chp->V[x] = chp->DT;
                chp->PC++;
                break;
            }
            if((chp->IR & 0x0FF) == 0x1E)
            {
                chp->I += chp->V[x];
                chp->PC++;
                break;
            }
            if((chp->IR & 0x0FF) == 0x0A)
            {
                chp->PC++;
                break;
                // first row
                if(keyboard(chp) != -1)
                {
                    chp->PC++;
                }
                break;
            }
            if((chp->IR & 0x00FF) == 0x15)
            {
                x = (chp->IR & 0x0F00) >> 8;
                chp->DT = chp->V[x];
                chp->PC++;
                break;
            }
            if((chp->IR & 0x00FF) == 0x18)
            {
                x = (chp->IR & 0x0F00) >> 8;
                chp->ST = chp->V[x];
                chp->PC++;
                break;
            }
            if((chp->IR & 0x00FF) == 0x29)
            {
                x = (chp->IR & 0x0F00) >> 8;
                chp->I = chp->V[x]*5;
                chp->PC++;
                break;
            }
            if((chp->IR & 0x00FF) == 0x33)
            {
                x = (chp->IR & 0x0F00) >> 8;
                chp->mem[chp->I] = (chp->V[x] / 100) % 10;
                chp->mem[chp->I+1] = (chp->V[x] / 10) % 10;
                chp->mem[chp->I+2] = (chp->V[x] / 1) % 10;
                chp->PC++;
                break;
            }
            if((chp->IR & 0x00FF) == 0x55)
            {
                    for(int i = 0; i <= ((chp->IR & 0x0F00) >> 8); i++)
                {
                    chp->mem[chp->I+i] = chp->V[i];
                }
                chp->PC++;
                break;
            }
            if((chp->IR & 0x00FF) == 0x65)
            {
                for(int i = 0; i <= ((chp->IR & 0x0F00) >> 8); i++)
                {
                        chp->V[i] = chp->mem[chp->I+i]; 
                }
                chp->PC++;
                break;
            }
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


int keyboard(Chip8* chp)
{
    unsigned x = (chp->IR & 0x0F00) >> 8;
    if((chp->IR & 0x00FF) == 0x0A)
    {
       if(IsKeyPressed(KEY_ONE))
       {
            chp->V[x] = 1;
       }
       else if(IsKeyPressed(KEY_TWO))
       {
            chp->V[x] = 2;
       }
       else if(IsKeyPressed(KEY_THREE))
       {
            chp->V[x] = 3;
       }
       else if(IsKeyPressed(KEY_FOUR))
       {
            chp->V[x] = 0xC;
       }
       // second row
       else if(IsKeyPressed(KEY_Q))
       {
            chp->V[x] = 4;
       }
       else if(IsKeyPressed(KEY_W))
       {
            chp->V[x] = 5;
       }
       else if(IsKeyPressed(KEY_E))
       {
            chp->V[x] = 6;
       }
       else if(IsKeyPressed(KEY_R))
       {
            chp->V[x] = 0xD;
       }
       // third row
       else if(IsKeyPressed(KEY_A))
       {
            chp->V[x] = 0x7;
       }
       else if(IsKeyPressed(KEY_S))
       {
            chp->V[x] = 0x8;
       }
       else if(IsKeyPressed(KEY_D))
       {
            chp->V[x] = 0x9;
       }
       else if(IsKeyPressed(KEY_F))
       {
            chp->V[x] = 0xE;
       }
       // last row
       else if(IsKeyPressed(KEY_Z))
       {
            chp->V[x] = 0xA;
       }
       else if(IsKeyPressed(KEY_X))
       {
            chp->V[x] = 0x0;
       }
       else if(IsKeyPressed(KEY_C))
       {
            chp->V[x] = 0xB;
       }
       else if(IsKeyPressed(KEY_V))
       {
            chp->V[x] = 0xF;
       }else
       {
            return -1;
       }
       return 0;
    }
    else if((chp->IR & 0x00FF) == 0xA1)
    {
       x = (chp->IR & 0x0F00) >> 8; 
       if(IsKeyDown(KEY_ONE) && chp->V[x] == 1)
       {
            chp->PC++;
       }
       else if(IsKeyDown(KEY_TWO) && chp->V[x] == 2)
       {
            chp->PC++;
       }
       else if(IsKeyDown(KEY_THREE) && chp->V[x] == 3)
       {
            chp->PC++;
       }
       else if(IsKeyDown(KEY_FOUR) && chp->V[x] == 0xc)
       {
            chp->PC++;
       }
       else if(IsKeyDown(KEY_Q) && chp->V[x] == 4)
       {
            chp->PC++;
       }
       else if(IsKeyDown(KEY_W) && chp->V[x] == 5)
       {
            chp->PC++;
       }
       else if(IsKeyDown(KEY_E) && chp->V[x] == 6)
       {
            chp->PC++;
       }
       else if(IsKeyDown(KEY_R) && chp->V[x] == 0xd)
       {
            chp->PC++;
       }
       else if(IsKeyDown(KEY_A) && chp->V[x] == 7)
       {
            chp->PC++;
       }
       else if(IsKeyDown(KEY_S) && chp->V[x] == 8)
       {
            chp->PC++;
       }
       else if(IsKeyDown(KEY_D) && chp->V[x] == 9)
       {
            chp->PC++;
       }
       else if(IsKeyDown(KEY_F) && chp->V[x] == 0xE)
       {
       }
       else if(IsKeyDown(KEY_Z) && chp->V[x] == 0xA)
       {
            chp->PC++;
       }
       else if(IsKeyDown(KEY_X) && chp->V[x] == 0)
       {
            chp->PC++;
       }
       else if(IsKeyDown(KEY_C) && chp->V[x] == 0xB)
       {
            chp->PC++;
       }
       else if(IsKeyDown(KEY_V) && chp->V[x] == 0xF)
       {
            chp->PC++;
       }
       else
       {
          chp->PC+=2; 
       }
       return 0;

    }
    else if((chp->IR & 0x00FF) == 0x9E)
    {
       x = (chp->IR & 0x0F00) >> 8; 
       if(IsKeyUp(KEY_ONE) && chp->V[x] == 1)
       {
            chp->PC++;
       }
       else if(IsKeyUp(KEY_TWO) && chp->V[x] == 2)
       {
            chp->PC++;
       }
       else if(IsKeyUp(KEY_THREE) && chp->V[x] == 3)
       {
            chp->PC++;
       }
       else if(IsKeyUp(KEY_FOUR) && chp->V[x] == 0xc)
       {
            chp->PC++;
       }
       else if(IsKeyUp(KEY_Q) && chp->V[x] == 4)
       {
            chp->PC++;
       }
       else if(IsKeyUp(KEY_W) && chp->V[x] == 5)
       {
            chp->PC++;
       }
       else if(IsKeyUp(KEY_E) && chp->V[x] == 6)
       {
            chp->PC++;
       }
       else if(IsKeyUp(KEY_R) && chp->V[x] == 0xd)
       {
            chp->PC++;
       }
       else if(IsKeyUp(KEY_A) && chp->V[x] == 7)
       {
            chp->PC++;
       }
       else if(IsKeyUp(KEY_S) && chp->V[x] == 8)
       {
            chp->PC++;
       }
       else if(IsKeyUp(KEY_D) && chp->V[x] == 9)
       {
            chp->PC++;
       }
       else if(IsKeyUp(KEY_F) && chp->V[x] == 0xE)
       {
       }
       else if(IsKeyUp(KEY_Z) && chp->V[x] == 0xA)
       {
            chp->PC++;
       }
       else if(IsKeyUp(KEY_X) && chp->V[x] == 0)
       {
            chp->PC++;
       }
       else if(IsKeyUp(KEY_C) && chp->V[x] == 0xB)
       {
            chp->PC++;
       }
       else if(IsKeyUp(KEY_V) && chp->V[x] == 0xF)
       {
            chp->PC++;
       }
       else
       {
          chp->PC+=2; 
       }
       return 0;
    }
    return -1;
}
