#include <stdint.h>
#include <stdio.h>

#include "cpu.h"
#include "basic_instructions.h"
#include "memory_reader.h"

uint8_t registers[9];
uint8_t flag_register;

void set_IME(uint16_t value)
{
}

uint8_t get_register(uint8_t index)
{
    return registers[index];
}

void set_register(uint8_t index, uint8_t value)
{
    registers[index] = value;
}

void set_register_rr(uint8_t dst_index, uint8_t src_index)
{
    registers[dst_index] = registers[src_index];
}

void increase_register(uint8_t index)
{
    registers[index] += 1;
}

void decrease_register(uint8_t index)
{
    registers[index] -= 1;
}

void add_register(uint8_t value)
{
    registers[A_REGISTER] += value;
}

void add_register_carry(uint8_t value)
{
    registers[A_REGISTER] += value;
}

void add_register_rr_carry(uint8_t index)
{
    registers[A_REGISTER] += registers[index];
}

void add_register_rr(uint8_t src_index)
{
    registers[A_REGISTER] += registers[src_index];
}

void sub_register(uint8_t value)
{
    registers[A_REGISTER] -= value;
}

void sub_register_rr(uint8_t index)
{
    uint8_t value;

    if (index == HL_REGISTER)
        value = read_memory(get_dual_register(index));
    else
        value = registers[index];

    registers[A_REGISTER] -= registers[value];
}

void sub_register_rr_carry(uint8_t src_index)
{
    registers[A_REGISTER] -= registers[src_index] + get_flag(FLAG_CARRY);
}

void or_register(uint8_t value)
{
    registers[A_REGISTER] |= value;
}

void or_register_rr(uint8_t index)
{
    uint8_t value;

    if (index == HL_REGISTER)
        value = read_memory(get_dual_register(index));
    else
        value = registers[index];

    registers[A_REGISTER] |= value;
}

void xor_register(uint8_t value)
{
    registers[A_REGISTER] = value;
}

void xor_register_rr(uint8_t index)
{
    uint8_t value;

    if (index == HL_REGISTER)
        value = read_memory(get_dual_register(index));
    else
        value = registers[index];

    registers[A_REGISTER] ^= value;
}

void and_register_rr(uint8_t index)
{
    uint8_t value;

    if (index == HL_REGISTER)
        value = read_memory(get_dual_register(index));
    else
        value = registers[index];

    registers[A_REGISTER] &= index;
}

void and_register(uint8_t value)
{
    registers[A_REGISTER] &= value;
}

void cp_register(uint8_t value)
{
    set_flag(FLAG_ZERO, value == get_register(A_REGISTER));
}

void set_register_bit(uint8_t register_index, uint8_t bit_index, uint8_t value)
{
    registers[register_index] = set_bit(registers[register_index], bit_index, value);
}

void rotate_register_left(uint8_t index)
{
    if (index == AF_REGISTER)
        perror("Rotation not supported in AF Register!");

    if (index == HL_REGISTER || index >= BC_REGISTER)
    {
        uint16_t value = get_dual_register(index);
        value = rotate_left_16(value);
        set_dual_register(index, value);
    }
    else
    {
        registers[index] = rotate_left(registers[index]);
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
        registers[index] = rotate_right(index);
        set_flag(FLAG_CARRY, registers[index] & 0x1);
    }
}

void rotate_register_right(uint8_t index)
{
    if (index == AF_REGISTER)
        perror("Rotation not supported in AF Register!");

    if (index == HL_REGISTER || index >= BC_REGISTER)
    {
        uint16_t value = get_dual_register(index);
        value = rotate_right_16(value);
        set_dual_register(index, value);
    }
    else
    {
        registers[index] = rotate_right(index);
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
        registers[index] = rotate_right(index);
        set_flag(FLAG_CARRY, registers[index] & 0x01);
    }
}

uint16_t get_dual_register(uint8_t index)
{
    switch (index)
    {
    case BC_REGISTER:
        return combine(registers[C_REGISTER], registers[B_REGISTER]);
    case DE_REGISTER:
        return combine(registers[E_REGISTER], registers[D_REGISTER]);
    case HL_REGISTER:
        return combine(registers[L_REGISTER], registers[H_REGISTER]);
    case AF_REGISTER:
        return combine(flag_register, registers[A_REGISTER]);
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
        registers[B_REGISTER] = combination_buffer[1];
        registers[C_REGISTER] = combination_buffer[0];
        break;
    case DE_REGISTER:
        from_combination(value);
        registers[E_REGISTER] = combination_buffer[0];
        registers[D_REGISTER] = combination_buffer[1];
        break;
    case HL_REGISTER:
        from_combination(value);
        registers[L_REGISTER] = combination_buffer[0];
        registers[H_REGISTER] = combination_buffer[1];
        break;
    case AF_REGISTER:
        from_combination(value);
        flag_register = combination_buffer[0];
        registers[A_REGISTER] = combination_buffer[1];
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
    flag_register = set_bit(flag_register, index, value);
}

uint8_t get_flag(uint8_t index)
{
    uint8_t mask = 0x01 << index;
    uint8_t value = flag_register & mask;

    value = value >> index;

    return value;
}
