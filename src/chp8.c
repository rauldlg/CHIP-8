typedef struct
{
    // general purpose registers VF and I are not used by programs
    unsigned char V[16];
    unsigned short I;
    unsigned char DT; // sound register
    unsigned char ST; // sound register
    unsigned short PC; // program counter
    unsigned char SP; // Stack Pointer
    unsigned short stack[16]; // Stack
} Chip8


