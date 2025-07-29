#include <stdbool.h>
#include <stdint.h>

#define B_REGISTER 0
#define C_REGISTER 1
#define D_REGISTER 2
#define E_REGISTER 3
#define H_REGISTER 5
#define L_REGISTER 6
#define HL_REGISTER 7
#define A_REGISTER 8
#define FLAG_REGISTER 10
#define BC_REGISTER 11
#define DE_REGISTER 12
#define AF_REGISTER 13

#define FLAG_ZERO 7
#define FLAG_SUB 6
#define FLAG_HALF_CARRY 5
#define FLAG_CARRY 4

void set_IME(uint16_t value);

uint8_t get_register(uint8_t index);

/// @brief Set register. If register is dual, write to the memory specified in such register. If wanted to write in a dual register, use set_dual_register
/// @param index 
/// @param value 
void set_register(uint8_t index, uint8_t value);

void set_dual_register(uint8_t index, uint16_t value);

void set_register_rr(uint8_t dst_index, uint8_t src_index);

void increase_register(uint8_t index);

void decrease_register(uint8_t index);

void add_register(uint8_t value);

void add_register_carry(uint8_t value);

void add_register_rr(uint8_t src_index);

void sub_register(uint8_t value);

void sub_register_rr(uint8_t src_index);

void add_register_rr_carry(uint8_t index);

void sub_register_carry(uint8_t value);

void sub_register_rr_carry(uint8_t src_index);

void and_register(uint8_t value);

void and_register_rr(uint8_t src_index);

void or_register(uint8_t value);

void xor_register(uint8_t value);

void cp_register(uint8_t value);

void set_register_bit(uint8_t register_index, uint8_t bit_index, uint8_t value);

void rotate_register_left(uint8_t index);

void rotate_register_left_carry(uint8_t index);

void rotate_register_right(uint8_t index);

void rotate_register_right_carry(uint8_t index);

uint16_t get_dual_register(uint8_t index);

void add_dual_register(uint8_t index, uint16_t value);

void sub_dual_register(uint8_t index, uint16_t value);

void set_flag(uint8_t index, uint8_t value);

void swap(uint8_t register_index);

uint8_t get_flag(uint8_t index);
