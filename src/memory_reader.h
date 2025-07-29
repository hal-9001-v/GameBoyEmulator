#include "cartridge.h"
#include <stdint.h>
#include <stdbool.h>


#define ROM_1_START 0x0000
#define ROM_1_END 0x3FFF
#define ROM_1_SIZE (ROM_1_END - ROM_1_START + 1)

#define ROM_2_START 0x4000
#define ROM_2_END 0x7FFF
#define ROM_2_SIZE (ROM_2_END - ROM_2_START + 1)

#define VRAM_START 0x8000
#define VRAM_END 0x9FFF
#define VRAM_SIZE (VRAM_END - VRAM_START + 1)

#define EXTERNAL_RAM_START 0xA000
#define EXTERNAL_RAM_END 0xBFFF
#define EXTERNAL_RAM_SIZE (EXTERNAL_RAM_END - EXTERNAL_RAM_START + 1)

#define WORK_RAM_1_START 0xC000
#define WORK_RAM_1_END 0xCFFF
#define WORK_RAM_1_SIZE (WORK_RAM_1_END - WORK_RAM_1_START + 1)

#define WORK_RAM_2_START 0xD000
#define WORK_RAM_2_END 0xDFFF
#define WORK_RAM_2_SIZE (WORK_RAM_2_END - WORK_RAM_2_START + 1)

#define ECHO_RAM_START 0xE000
#define ECHO_RAM_END 0xFDFF
#define ECHO_RAM_SIZE (ECHO_RAM_END - ECHO_RAM_START + 1)

#define OAM_START 0xFE00
#define OAM_END 0xFE9F
#define OAM_SIZE (OAM_END - OAM_START + 1)

#define NOT_USABLE_START 0xFEA0
#define NOT_USABLE_END 0xFEFF
#define NOT_USABLE_SIZE (NOT_USABLE_END - NOT_USABLE_START + 1)

#define IO_REGISTER_START 0xFF00
#define IO_REGISTER_END 0xFF7F
#define IO_REGISTER_SIZE (IO_REGISTER_END - IO_REGISTER_START + 1)

#define HRAM_START 0xFF80
#define HRAM_END 0xFFFE
#define HRAM_SIZE (HRAM_END - HRAM_START + 1)

void list_roms();

struct cartridge* get_cartridge();

uint8_t read_memory(uint16_t address);

uint16_t read_memory_16(uint16_t address);

void write_memory(uint16_t address, uint8_t value);

void write_memory_16_combine(uint16_t address, uint8_t a, uint8_t b);

void write_memory_16(uint16_t address, uint16_t value);

uint8_t access_memory(uint16_t address, bool read, uint8_t value);