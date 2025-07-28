#include "cpu.h"
#include "memory_reader.h"
#include "decoder.h"
#include <stdio.h>
#include <stdint.h>

void init()
{
    list_roms();
    get_cartridge(12);
}

void tick()
{

}