#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include "memory_reader.h"


#define INTERRUPT 0xFFFF

struct cartridge *cartridge;
uint8_t video_ram[VRAM_SIZE];
uint8_t work_ram_1[WORK_RAM_1_SIZE];
uint8_t work_ram_2[WORK_RAM_2_SIZE];
uint8_t external_ram[EXTERNAL_RAM_SIZE];
uint8_t oam[OAM_SIZE];
uint8_t echo_ram[ECHO_RAM_SIZE];
uint8_t not_usable_rom[NOT_USABLE_SIZE];
uint8_t io_register[NOT_USABLE_SIZE];
uint8_t hram[HRAM_SIZE];
uint8_t interrupt;

void list_roms()
{
    DIR *folder;
    struct dirent *entry;
    int files = 0;

    folder = opendir(".");
    if (folder == NULL)
    {
        perror("Unable to read directory");
    }

    while ((entry = readdir(folder)))
    {
        files++;

        printf("File %3d: %s\n",
               files,
               entry->d_name);
    }

    closedir(folder);
}

uint8_t read_memory(uint16_t address)
{
    return access_memory(address, true, 0);
}

uint16_t read_memory_16(uint16_t address)
{
    uint16_t value = access_memory(address, true, 0) << 8;
    value += access_memory(address + 1, true, 0);
    return value;
}

void write_memory(uint16_t address, uint8_t value)
{
    access_memory(address, false, value);
}

void write_memory_16_combine(uint16_t address, uint8_t a, uint8_t b)
{
    access_memory(address, false, a);
    access_memory(address + 1, false, b);
}

void write_memory_16(uint16_t address, uint16_t value)
{
    
}

struct cartridge *get_cartridge(int index)
{
    DIR *folder;
    struct dirent *entry;
    int files = 0;

    folder = opendir(".");
    if (folder == NULL)
    {
        perror("Unable to read directory");
    }

    while ((entry = readdir(folder)))
    {
        files++;

        if (files == index)
        {
            FILE *fd = fopen(entry->d_name, "r");

            if (fd == NULL)
            {
                perror("Cant read file");
                closedir(folder);
                return NULL;
            }

            int size = 32 * 1024;
            char *content = malloc(size * sizeof(char));
            if (content == NULL)
            {
                perror("malloc failed");
                break;
            }

            size_t bytesRead = fread(content, 1, size, fd);
            if (bytesRead == 0)
            {
                perror("fread failed or empty file");
                free(content);
                fclose(fd);
                closedir(folder);
                return NULL;
            }

            struct cartridge *result = malloc(sizeof(struct cartridge));
            if (result == NULL)
            {
                perror("malloc failed");
                break;
            }

            result->name = strdup(entry->d_name);
            if (result->name == NULL)
            {
                perror("strdup failed");
                free(content);
                free(result);
                fclose(fd);
                closedir(folder);
                return NULL;
            }

            result->rom_content = content;

            fclose(fd);
            closedir(folder);
            cartridge = result;
            return result;
        }
    }

    closedir(folder);
    return NULL;
}

// https://gbdev.io/pandocs/Memory_Map.html
uint8_t access_memory(uint16_t address, bool read, uint8_t value)
{
    if (address < ROM_1_START)
    {
        perror("Out of range memory");
        return 0x0000;
    }

    if (address <= ROM_1_END)
    {
        // 16KiB ROM Bank
        if (read)
        {
            return cartridge->rom_content[address - ROM_1_START];
        }
        else
        {
            perror("Tried to write in ROM!");
        }
    }
    else if (address <= ROM_2_END)
    {
        // 16 KiB ROM Bank 01-NN
        if (read)
        {
            return cartridge->rom_content[address - ROM_2_START];
        }
        else
        {
            perror("Tried to write in ROM!");
        }
    }
    else if (address <= VRAM_END)
    {
        // 8 KiB Video RAM (VRAM)
        // 16KiB ROM Bank
        if (read)
        {
            return video_ram[address - VRAM_START];
        }
        else
        {
            video_ram[address - VRAM_START] = value;
        }
    }
    else if (address <= EXTERNAL_RAM_END)
    {
        // 8 KiB External RAM
        if (read)
        {
            return external_ram[address - EXTERNAL_RAM_START];
        }
        else
        {
            external_ram[address - EXTERNAL_RAM_START] = value;
        }
    }
    else if (address <= WORK_RAM_1_END)
    {
        // 4 KiB Work RAM (WRAM)
        if (read)
        {
            return work_ram_1[address - WORK_RAM_1_START];
        }
        else
        {
            work_ram_1[address - WORK_RAM_1_START] = value;
        }
    }
    else if (address <= WORK_RAM_2_END)
    {
        // 4 KiB Work RAM (WRAM)
        if (read)
        {
            return work_ram_2[address - WORK_RAM_2_START];
        }
        else
        {
            work_ram_2[address - WORK_RAM_2_START] = value;
        }
        // 4 KiB Work RAM (WRAM)
    }
    else if (address <= ECHO_RAM_END)
    {
        // OEcho RAM (mirror of C000–DDFF)
        if (read)
        {
            return echo_ram[address - ECHO_RAM_START];
        }
        else
        {
            echo_ram[address - ECHO_RAM_START] = value;
        }
    }
    else if (address <= OAM_END)
    {
        // Object attribute memory (OAM)
        if (read)
        {
            return oam[address - OAM_START];
        }
        else
        {
            oam[address - OAM_START] = value;
        }
    }
    else if (address < NOT_USABLE_END)
    {

        // Not Usable
    }
    else if (address < IO_REGISTER_END)
    {
        // I/O Registers
        if (read)
        {
            return io_register[address - IO_REGISTER_START];
        }
        else
        {
            io_register[address - IO_REGISTER_START] = value;
        }
    }
    else if (address <= HRAM_END)
    {
        // High RAM (HRAM)
        if (read)
        {
            return hram[address - HRAM_START];
        }
        else
        {
            hram[address - HRAM_START] = value;
        }
    }
    else if (address == INTERRUPT)
    {
        // Interrupt Enable register (IE)
        if (read)
        {
            return interrupt;
        }
        else
        {
            interrupt = value;
        }
    }
    else
    {
        perror("Address out of bounds!");
        return 0x0000;
    }
}