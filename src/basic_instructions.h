
#ifndef BASIC_H
#define BASIC_H
#include <stdint.h>

extern uint8_t combination_buffer[2];
#endif

/// @brief Combines a and b into a single 16bit value. Given a = 00000001 and b 10000010, the result is 00000001-10000010
/// @param a Greater values
/// @param b Lesser values
/// @return
uint16_t combine(uint8_t a, uint8_t b);

/// @brief
/// @param combined
/// @return Returns 2 sized array, stored in combination_buffer. First value comes from the bigger values. 1010000001 -> [0] = 00000101; [1] = 00000001
void from_combination(uint16_t combined);

uint8_t set_bit(uint8_t source, uint8_t index, uint8_t value);

uint16_t set_bit_16(uint16_t source, uint8_t index, uint8_t value);

uint8_t rotate_left(uint8_t target);

uint16_t rotate_left_16(uint16_t target);

uint8_t rotate_right(uint8_t target);

uint16_t rotate_right_16(uint16_t target);
