#include "chp8.h"
#include <string.h>
#include <stdio.h>
//#include <raylib.h>

int sizeof_rom(FILE* rom);

int main(int argc, char** argv)
{
        if (argc != 2)
        {
            fprintf(stderr, "usage: ./%s <file.chp8>\n", argv[0]);
            return -1;
        }
        // initializing memory
        unsigned char ram[4096];
        memset(ram, 0, 4096*sizeof(char));
       
        // read rom in ram
        FILE* rom = fopen(argv[1], "r");
        fread(&ram[0x200], sizeof(char), 4096-0x200, rom);
        // close rom stream
        fclose(rom);

        // initialize chip-8 interpreter
        Chip8 chp8;
        unsigned short* start_ram = (unsigned short*)&ram;

        initialize_chip8(&chp8, start_ram);
 //       InitWindow(640, 320, "CHIP-8 Emulator");
   //     SetTargetFPS(60);
        for(int i = 0; i < 33; i++)
        {
            fetch(&chp8);
            decode(&chp8);
            execute(&chp8);
        }

    return 0;
}


int sizeof_rom(FILE* rom)
{
    char c = 0;
    int counter = 0;
    while((c = getc(rom)) != EOF)
    {
        counter++;
    }
    return counter;
}


