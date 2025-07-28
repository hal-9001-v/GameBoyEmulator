#include "cpu.h"
#include "basic_instructions.h"

uint8_t combination_buffer[2];

/// @brief Combines a and b into a single value as little indian. if a = 00000001 and b 10000010, result is 1000001000000001
/// @param a Lesser values
/// @param b Greater values
/// @return
uint16_t combine(uint8_t a, uint8_t b)
{
    uint16_t result = a;
    result |= ((uint16_t)b << 8);
    return result;
}

/// @brief
/// @param combined
/// @return Returns 2 sized array. First value comes from the lesser values(little indian). 1010000001 -> [0] = 00000001; [1] = 00000101
void from_combination(uint16_t combined)
{
    combination_buffer[0] = combined & 0x00FF;
    combination_buffer[1] = (combined & 0xFF00) >> 8;
}

uint8_t set_bit(uint8_t source, uint8_t index, uint8_t value)
{
    if (value == 0)
        source &= ~(1 << index);
    else
        source |= (1 << index);

    return source;
}

uint16_t set_bit_16(uint16_t source, uint8_t index, uint8_t value)
{
    if (value == 0)
        source &= ~(1 << index);
    else
        source |= (1 << index);

    return source;
}

uint8_t rotate_left(uint8_t target)
{
    uint8_t last_bit = target;
    last_bit = (last_bit & 0x80) >> 7;

    target = target << 1;
    target |= last_bit;

    return target;
}

uint8_t rotate_right(uint8_t target)
{
    uint8_t first_bit = target;
    first_bit = (first_bit & 0x01) << 7;

    target = target >> 1;
    target |= first_bit;

    return target;
}

uint16_t rotate_left_16(uint16_t target)
{
    uint16_t last_bit = target;
    last_bit = (last_bit & 0x8000) >> 15;

    target = target << 1;
    target |= last_bit;

    return target;
}

uint16_t rotate_right_16(uint16_t target)
{
    uint16_t last_bit = target;
    last_bit = (last_bit & 0x0001) << 15;

    target = target >> 1;
    target |= last_bit;

    return target;
}