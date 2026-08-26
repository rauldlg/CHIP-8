#include "chp8.h"
#include <math.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <raylib.h>
#include <stdlib.h>

#define WIDTH 64
#define HEIGHT 32
#define SCALE 10
#define FPS 300

int sizeof_rom(FILE* rom);

void draw_display(Chip8* chp);

int main(int argc, char** argv)
{
    srand(time(NULL));
        if (argc != 2)
        {
            fprintf(stderr, "usage: ./%s <file.chp8>\n", argv[0]);
            return -1;
        }
        
        unsigned char ram[4096];
        memset(ram, 0, 4096*sizeof(char)); // initializing memory
       
        
        FILE* rom = fopen(argv[1], "r"); // read rom in ram
        fread(&ram[0x200], sizeof(char), 4096-0x200, rom);
        fclose(rom); // close rom stream

        
        Chip8 chp8;
        initialize_chip8(&chp8, ram); // initialize chip-8 interpreter

        InitWindow(WIDTH*SCALE+300, HEIGHT*SCALE, "CHIP-8 Interpreter");
        InitAudioDevice();
        SetAudioStreamBufferSizeDefault(4096);

        float buffer[4096] = {};

        AudioStream stream = LoadAudioStream(44100, 32, 1);
        float pan = 0.0f;
        SetAudioStreamPan(stream, pan);

        int sineFrequency = 440;
        int newSineFrequency = 440;
        int sineIndex = 0;

        SetTargetFPS(FPS);

        while(!WindowShouldClose())
        {
            fetch(&chp8);
            decode(&chp8);
            execute(&chp8);
            if(chp8.DT > 0) chp8.DT--;
            if(chp8.ST > 0)
            {
                chp8.ST--;
                // play audio
                PlayAudioStream(stream);
                for (int i = 0; i < 4096; i++)
                {
                    int wavelength = 44100/sineFrequency;
                    buffer[i] = sinf(2*PI*sineIndex/wavelength);
                    sineIndex++;
                    if (sineIndex >= wavelength)
                        {
                            sineFrequency = newSineFrequency;
                            sineIndex = 0;
                        }
                    UpdateAudioStream(stream, buffer, 4096);
                }
            }
            else
            {
                StopAudioStream(stream);
            }
            BeginDrawing();
               ClearBackground(BLACK); 
               draw_display(&chp8);
            EndDrawing();
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

void draw_display(Chip8* chp)
{

    int w, h;
    w = 0;
    h = 0;

    for(int y = 0; y < 32; y++)
    {
        for(int x = 0; x < 64; x++)
        {
            if(chp->display[x][y])
            {
                DrawRectangle(w, h, SCALE, SCALE, WHITE);
                w+=SCALE;
            }
            else
            {
                DrawRectangle(w, h, SCALE, SCALE, BLACK);
                w+=10;
            }
        
        }
        h+=10;
        w = 0;
    }
}
