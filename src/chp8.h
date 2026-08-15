
typedef struct
{
    // general purpose registers VF and I are not used by programs
    unsigned char V[16];
    unsigned short I;
    unsigned char DT; // sound register
    unsigned char ST; // sound register
    unsigned short* PC; // program counter
    unsigned short* SP; // Stack Pointer
    unsigned short stack[16]; // Stack
    unsigned short IR;
} Chip8;

void initialize_chip8(Chip8* chp, unsigned short* mem);

unsigned short fetch(Chip8* chp);

void decode(Chip8* chp, unsigned short* opcode);
