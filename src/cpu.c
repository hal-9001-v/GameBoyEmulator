#include <stdint.h>
#include <stdio.h>

#include "cpu.h"
#include "basic_instructions.h"
#include "memory_reader.h"

uint8_t registers[10];

bool is_dual_register(uint8_t index)
{
    if (index == HL_REGISTER)
        return true;
    if (index == BC_REGISTER)
        return true;
    if (index == DE_REGISTER)
        return true;
    if (index == AF_REGISTER)
        return true;

    return false;
}

void set_IME(uint16_t value)
{
}

uint8_t get_register(uint8_t index)
{
    if (is_dual_register(index))
        return read_memory(get_dual_register(index));

    return registers[index];
}

void set_register(uint8_t index, uint8_t value)
{
    if (is_dual_register(index))
        write_memory(get_dual_register(index), value);
    else
        registers[index] = value;
}

void set_register_rr(uint8_t dst_index, uint8_t src_index)
{
    set_register(dst_index, get_register(src_index));
}

void increase_register(uint8_t index)
{
    set_register(index, get_register(index) + 1);
}

void decrease_register(uint8_t index)
{
    set_register(index, get_register(index) - 1);
}

void add_register(uint8_t value)
{
    set_register(A_REGISTER, get_register(A_REGISTER) + value);
}

void add_register_carry(uint8_t value)
{
    add_register(value);
}

void add_register_rr_carry(uint8_t index)
{
    add_register_carry(get_register(index));
}

void add_register_rr(uint8_t index)
{
    set_register(A_REGISTER, get_register(index));
}

void sub_register(uint8_t value)
{
    set_register(A_REGISTER, get_register(A_REGISTER) - value);
}

void sub_register_rr(uint8_t index)
{
    sub_register(get_register(index));
}

void sub_register_rr_carry(uint8_t index)
{
    sub_register(get_register(index) + get_flag(FLAG_CARRY));
}

void sub_register_carry(uint8_t value)
{
    set_register(A_REGISTER, get_register(A_REGISTER) - value);
}

void or_register(uint8_t value)
{
    set_register(A_REGISTER, get_register(A_REGISTER) | value);
}

void or_register_rr(uint8_t index)
{
    or_register(get_register(index));
}

void xor_register(uint8_t value)
{
    set_register(A_REGISTER, get_register(A_REGISTER) ^ value);
}

void xor_register_rr(uint8_t index)
{
    xor_register(get_register(index));
}

void and_register_rr(uint8_t index)
{
    and_register(get_register(index));
}

void and_register(uint8_t value)
{
    set_register(A_REGISTER, get_register(A_REGISTER) & value);
}

void cp_register(uint8_t value)
{
    set_flag(FLAG_ZERO, value == get_register(A_REGISTER));
}

void set_register_bit(uint8_t register_index, uint8_t bit_index, uint8_t value)
{
    set_register(register_index, set_bit(get_register(register_index), bit_index, value));
}

void rotate_register_left(uint8_t index)
{
    if (index == AF_REGISTER)
        perror("Rotation not supported in AF Register!");

    if (is_dual_register(index))
    {
        uint16_t value = get_dual_register(index);
        value = rotate_left_16(value);
        set_dual_register(index, value);
    }
    else
    {
        set_register(index, rotate_left(get_register(index)));
    }
}

void rotate_register_left_carry(uint8_t index)
{
    if (index == AF_REGISTER)
        perror("Rotation not supported in AF Register!");

    if (index == HL_REGISTER || index >= BC_REGISTER)
    {
        uint16_t value = get_dual_register(index);
        value = rotate_left_16(value);
        set_flag(FLAG_CARRY, value & 0x0001);
        set_dual_register(index, value);
    }
    else
    {
        set_register(index, rotate_left(get_register(index)));
        set_flag(FLAG_CARRY, get_register(index) & 0x1);
    }
}

void rotate_register_right(uint8_t index)
{
    if (index == HL_REGISTER || index >= BC_REGISTER)
    {
        uint16_t value = get_dual_register(index);
        value = rotate_right_16(value);
        set_dual_register(index, value);
    }
    else
    {
        set_register(index, rotate_right(get_register(index)));
    }
}

void rotate_register_right_carry(uint8_t index)
{
    if (index == AF_REGISTER)
        perror("Rotation not supported in AF Register!");

    if (index == HL_REGISTER || index >= BC_REGISTER)
    {
        uint16_t value = get_dual_register(index);
        set_flag(FLAG_CARRY, value & 0x0001);
        value = rotate_right_16(value);
        set_dual_register(index, value);
    }
    else
    {
        set_register(index, rotate_right(get_register(index)));
        set_flag(FLAG_CARRY, get_register(index) & 0x01);
    }
}

uint16_t get_dual_register(uint8_t index)
{
    switch (index)
    {
    case BC_REGISTER:
        return combine(get_register(C_REGISTER), get_register(B_REGISTER));
    case DE_REGISTER:
        return combine(get_register(E_REGISTER), get_register(D_REGISTER));
    case HL_REGISTER:
        return combine(get_register(L_REGISTER), get_register(H_REGISTER));
    case AF_REGISTER:
        return combine(get_register(FLAG_REGISTER), get_register(A_REGISTER));
    default:
        perror("Such register does not exist!");
        return -1;
    }
}

void set_dual_register(uint8_t index, uint16_t value)
{
    switch (index)
    {
    case BC_REGISTER:
        from_combination(value);
        set_register(B_REGISTER, combination_buffer[1]);
        set_register(C_REGISTER, combination_buffer[0]);
        break;
    case DE_REGISTER:
        from_combination(value);
        set_register(E_REGISTER, combination_buffer[0]);
        set_register(D_REGISTER, combination_buffer[1]);
        break;
    case HL_REGISTER:
        from_combination(value);
        set_register(L_REGISTER, combination_buffer[0]);
        set_register(H_REGISTER, combination_buffer[1]);
        break;
    case AF_REGISTER:
        from_combination(value);
        set_register(FLAG_REGISTER, combination_buffer[0]);
        set_register(A_REGISTER, combination_buffer[1]);
        break;
    default:
        perror("Such register does not exist!");
        break;
    }
}

void add_dual_register(uint8_t index, uint16_t value)
{
    value += get_dual_register(index);
    set_dual_register(index, value);
}

void sub_dual_register(uint8_t index, uint16_t value)
{
    value = get_dual_register(index) - value;
    set_dual_register(index, value);
}

void set_flag(uint8_t index, uint8_t value)
{
    set_register(FLAG_REGISTER, set_bit(get_register(FLAG_REGISTER), index, value));
}

void swap(uint8_t register_index)
{
    uint8_t value = get_register(register_index);

    uint8_t left = value & 0xF0;

    value = value >> 4;
    value |= left;

    set_register(register_index, value);
}

uint8_t get_flag(uint8_t index)
{
    uint8_t mask = 0x01 << index;
    uint8_t value = get_register(FLAG_REGISTER) & mask;

    value = value >> index;

    return value;
}
