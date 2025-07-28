#include "cartridge.h"
#include <stdint.h>
#include <stdbool.h>

void list_roms();

struct cartridge* get_cartridge();

uint8_t read_memory(uint16_t address);

uint16_t read_memory_16(uint16_t address);

void write_memory(uint16_t address, uint8_t value);

void write_memory_16_combine(uint16_t address, uint8_t a, uint8_t b);

void write_memory_16(uint16_t address, uint16_t value);

uint8_t access_memory(uint16_t address, bool read, uint8_t value);