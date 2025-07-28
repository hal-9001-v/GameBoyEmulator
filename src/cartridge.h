#include <stdint.h>

struct cartridge
{
    uint8_t* rom_content;
    unsigned int content_size;
    unsigned char* name;
    unsigned int ram_size;
};