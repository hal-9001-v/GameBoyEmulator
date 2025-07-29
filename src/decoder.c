#include "decoder.h"
#include "cpu.h"
#include "memory_reader.h"

#include <stdio.h>
uint16_t program_counter;
uint16_t stack_pointer = 0xFFFE;

uint8_t get_range_value(uint8_t full, uint8_t start, uint8_t end)
{
    uint8_t length = end - start + 1;
    full = full >> start;
    uint8_t mask = (1 << length) - 1;
    return full & mask;
}


void halt()
{
}

void nop()
{
}

void return_from_stack()
{
    program_counter = stack_pointer;
}

void ld_block(uint8_t code)
{
    if (code == 0x76)
        return;

    uint8_t src = get_range_value(code, 0, 2);
    uint8_t value = get_range_value(code, 3, 5);

    set_register(src, value);
}

void decode_16_op()
{
    program_counter++;

    uint8_t code = read_memory(program_counter);

    if (code < 0x08)
    {
        uint8_t index = get_range_value(code, 0, 2);
        // RLC
        if (index == 0x06)
            rotate_register_left_carry(HL_REGISTER);
        else
            rotate_register_left_carry(index);
    }
    else if (code < 0x10)
    {
        uint8_t index = get_range_value(code, 0, 2);
        // RCC
        if (index == 0x06)
            rotate_register_right_carry(HL_REGISTER);
        else
            rotate_register_right_carry(index);
    }
    else if (code < 0x18)
    {
        // RL
        uint8_t index = get_range_value(code, 0, 2);
        if (index == 0x06)
            rotate_register_left(HL_REGISTER);
        else
            rotate_register_left(index);
    }
    else if (code < 0x20)
    {
        // RR
        uint8_t index = get_range_value(code, 0, 2);
        if (index == 0x06)
            rotate_register_right(HL_REGISTER);
        else
            rotate_register_right(index);
    }
    else if (code < 0x28)
    {
        // SLA
        uint8_t index = get_range_value(code, 0, 2);
        if (index == 0x06)
        {
            rotate_register_left_carry(HL_REGISTER);
            uint16_t value = get_dual_register(HL_REGISTER) & 0xFE;
            set_dual_register(HL_REGISTER, value);
        }
        else
        {
            rotate_register_left_carry(index);
            uint8_t value = get_register(index);
            value = value & 0xFE;
            set_register(index, value);
        }
    }
    else if (code < 0x30)
    {
        // SRA
        uint8_t index = get_range_value(code, 0, 2);
        if (index == 0x06)
        {
            rotate_register_right_carry(HL_REGISTER);
            uint16_t value = get_dual_register(HL_REGISTER) & 0x7F;
            set_dual_register(HL_REGISTER, value);
        }
        else
        {
            rotate_register_left_carry(index);
            uint8_t value = get_register(index);
            value = value & 0xFE;
            set_register(index, value);
        }
    }
    else if (code < 0x38)
    {
        
        // SWAP

    }
    else if (code < 0x40)
    {
        // SRL
    }
    else if (code < 0x80)
    {
        // BIT
    }
    else if (code < 0xC0)
    {
        // RES
    }
    else
    {
        // SET
        uint8_t index = get_range_value(code, 0, 2);
    }
}

void a_operation_block(uint8_t code)
{
    if (code < 0x80)
        return;

    uint8_t src = get_range_value(code, 0, 2);
    if (code < 0x88)
    {
        // ADD
        add_register_rr(src);
    }
    else if (code < 0x90)
    {
        // ADC
        add_register_rr_carry(src);
    }
    else if (code < 0x98)
    {
        // SUB
        sub_register_rr(src);
    }
    else if (code < 0xA0)
    {
        // SBC
        sub_register_carry(src);
    }
    else if (code < 0xA8)
    {
        // AND
        and_register(src);
    }
    else if (code < 0xB0)
    {
        // XOR
        xor_register(src);
    }
    else if (code < 0xB8)
    {
        // OR
        or_register(src);
    }
    else if (code < 0xC0)
    {
        // CP
        cp_register(src);
    }

    perror("Code ouf of block!");
}

void push_block(uint8_t code)
{
    uint8_t value = get_range_value(code, 4, 5);
    write_memory_16(stack_pointer, get_dual_register(BC_REGISTER + value));
}

uint16_t decode(uint8_t code)
{
    uint16_t address;
    uint8_t value;

    if (code < 0x40)
    {
        switch (code)
        {
        case 0x00:
            // NOP
            nop();
            break;
        case 0x01:
            // LD BC D16: Load the 2 bytes of immediate data into register pair BC.
            program_counter++;
            set_dual_register(BC_REGISTER, read_memory_16(program_counter));
            break;
        case 0x02:
            // LD (BC), A: Store the contents of register A in the memory location specified by register pair BC.
            write_memory(get_dual_register(BC_REGISTER), get_register(A_REGISTER));
            break;
        case 0x03:
            // INC BC: Increment the contents of register pair BC by 1.
            increase_register(BC_REGISTER);
            break;
        case 0x04:
            // INC B: Increment the contents of register B by 1.
            increase_register(B_REGISTER);
            break;
        case 0x05:
            // DEC B: Decrement the contents of register B by 1.
            decrease_register(B_REGISTER);
            break;
        case 0x06:
            // LD B, d8: Load the 8-bit immediate operand d8 into register B.
            program_counter++;
            set_register(B_REGISTER, read_memory(program_counter));
            break;
        case 0x07:
            // RLCA: Rotate the contents of register A to the left.
            rotate_register_left(A_REGISTER);
            break;
        case 0x08:
            /*LD (a16), SP: Store the lower byte of stack pointer SP at the address specified by the 16-bit
            immediate operand a16, and store the upper byte of SP at address a16 + 1.*/
            program_counter++;
            write_memory_16(read_memory_16(program_counter), stack_pointer);
            break;
        case 0x09:
            /*ADD HL, BC: Add the contents of register pair BC to the contents of register pair HL,
             and store the results in register pair HL */
            add_dual_register(HL_REGISTER, get_dual_register(BC_REGISTER));
            break;
        case 0x0A:
            // LD A, (BC): Load the 8-bit contents of memory specified by register pair BC into register A.
            set_register(A_REGISTER, read_memory(get_dual_register(BC_REGISTER)));
            break;
        case 0x0B:
            // DEC BC: Decrement the contents of register pair BC by 1.
            decrease_register(BC_REGISTER);
            break;
        case 0x0C:
            // INC C: Increment the contents of register C by 1.
            increase_register(C_REGISTER);
        case 0x0D:
            // DEC C: Decrement the contents of register C by 1.
            decrease_register(C_REGISTER);
            break;
        case 0x0E:
            // LD C, d8: Load the 8-bit immediate operand d8 into register C.
            program_counter++;
            set_register(C_REGISTER, read_memory(program_counter));
            break;
        case 0x0F:
            // RRCA: Rotate the contents of register A to the right.
            rotate_register_right(A_REGISTER);
            break;
        case 0x10:
            // Stop instruction.
            break;
        case 0x11:
            // LD DE, d16: Load the 2 bytes of immediate data into register pair DE.
            program_counter++;
            set_dual_register(DE_REGISTER, read_memory_16(program_counter));
            break;
        case 0x12:
            // LD (DE), A: Store the contents of register A in the memory location specified by register pair DE.
            program_counter++;
            write_memory(read_memory_16(program_counter), get_register(A_REGISTER));
            break;
        case 0x13:
            // INC DE: Increment the contents of register pair DE by 1.
            add_dual_register(DE_REGISTER, 1);
            break;
        case 0x14:
            // INC D Increment the contents of register D by 1.
            increase_register(D_REGISTER);
            break;
        case 0x15:
            // DEC D: Decrement the contents of register D by 1.
            decrease_register(D_REGISTER);
            break;
        case 0x16:
            // LD H, d8: Load the 8-bit immediate operand d8 into register H.
            program_counter++;
            set_register(H_REGISTER, read_memory(program_counter));
            break;
        case 0x17:
            // RLA: Rotate the contents of register A to the left, through the carry (CY) flag.
            rotate_register_left(A_REGISTER);
            set_register_bit(A_REGISTER, 0, get_flag(FLAG_CARRY));
            break;
        case 0x18:
            // JR s8: Jump s8 steps from the current address in the program counter (PC).
            program_counter++;
            program_counter += read_memory_16(program_counter);
        case 0x19:
            // ADD HL, DE: Add the contents of register pair DE to the contents of register pair HL, and store the results in register pair HL.
            add_dual_register(HL_REGISTER, get_dual_register(DE_REGISTER));
            break;
        case 0x1A:
            // D A, (DE): Load the 8-bit contents of memory specified by register pair DE into register A.
            set_register(A_REGISTER, read_memory(get_dual_register(DE_REGISTER)));
            break;
        case 0x1B:
            // DEC DE: Decrement the contents of register pair DE by 1.
            sub_dual_register(DE_REGISTER, 1);
            break;
        case 0x1C:
            // INC E: Increment the contents of register E by 1.
            increase_register(E_REGISTER);
            break;
        case 0x1D:
            // DEC E: Decrement the contents of register E by 1.
            decrease_register(E_REGISTER);
            break;
        case 0x1E:
            // LD E, d8:Load the 8-bit immediate operand d8 into register E.
            program_counter++;
            set_register(E_REGISTER, read_memory(program_counter));
            break;
        case 0x1F:
            // RRA: Rotate the contents of register A to the right, through the carry (CY) flag.
            rotate_register_right(A_REGISTER);
            set_register_bit(A_REGISTER, 7, get_flag(FLAG_CARRY));
            break;
        case 0x20:
            // JR NZ, s8: If the Z flag is 0, jump s8 steps from the current address stored in the program counter (PC).
            //  If not, the instruction following the current JP instruction is executed (as usual).
            program_counter++;
            if (get_flag(FLAG_ZERO) == 0)
            {
                program_counter += read_memory(program_counter);
            }

            break;
        case 0x21:
            // LD HL, d16: Load the 2 bytes of immediate data into register pair HL.
            program_counter++;
            set_dual_register(HL_REGISTER, read_memory_16(program_counter));
            break;
        case 0x22:
            // LD (HL+), A: Store the contents of register A into the memory location specified by register pair HL, and simultaneously increment the contents of HL.
            write_memory(get_dual_register(HL_REGISTER), get_register(A_REGISTER));
            add_dual_register(HL_REGISTER, 1);
            break;
        case 0x23:
            // INC HL: Increment the contents of register pair HL by 1.
            add_dual_register(HL_REGISTER, 1);
            break;
        case 0x24:
            // INC H: Increment the contents of register H by 1.
            increase_register(H_REGISTER);
            break;
        case 0x25:
            // DEC H: Decrement the contents of register H by 1.
            decrease_register(H_REGISTER);
            break;
        case 0x26:
            // LD H, d8: Load the 8-bit immediate operand d8 into register H.
            program_counter++;
            set_register(H_REGISTER, read_memory(program_counter));
            break;
        case 0x27:
            // DAA: Adjust the accumulator (register A) too a binary-coded decimal (BCD) number after BCD addition and subtraction operations.

            // TODO
            break;
        case 0x28:
            // JR Z, s8: if the Z flag is 1, jump s8 steps from the current address stored in the program counter (PC).
            // If not, the instruction following the current JP instruction is executed (as usual).
            program_counter++;
            if (get_flag(FLAG_ZERO) == 1)
            {
                program_counter += read_memory(program_counter);
            }
            break;
        case 0x29:
            // ADD HL, DE: Add the contents of register pair DE to the contents of register pair HL, and store the results in register pair HL.
            add_dual_register(HL_REGISTER, get_register(DE_REGISTER));
            break;
        case 0x2A:
            // LD A, (DE): Load the 8-bit contents of memory specified by register pair DE into register A
            set_register_rr(A_REGISTER, DE_REGISTER);
            break;
        case 0x2B:
            // DEC DE: Decrement the contents of register pair DE by 1.
            sub_dual_register(DE_REGISTER, 1);
            break;
        case 0x2C:
            // INC E: Increment the contents of register E by 1.
            increase_register(E_REGISTER);
            break;
        case 0x2D:
            // DEC E: Decrement the contents of register E by 1
            decrease_register(E_REGISTER);
            break;
        case 0x2E:
            // LD E, d8: Load the 8-bit immediate operand d8 into register E.
            program_counter++;
            set_register(E_REGISTER, read_memory(program_counter));
            break;
        case 0x2F:
            // RRA: Rotate the contents of register A to the right, through the carry (CY) flag.

            // rotate_register_right(A_REGISTER);

            break;
        case 0x30:
            // JR NC, s8: If the CY flag is 0, jump s8 steps from the current address stored in the program counter (PC).
            // If not, the instruction following the current JP instruction is executed (as usual).
            program_counter++;
            if (get_flag(FLAG_CARRY) == 0)
            {
                program_counter += read_memory(program_counter);
            }
            break;
        case 0x31:
            // LD SP, d16: Load the 2 bytes of immediate data into register pair SP.
            program_counter++;
            stack_pointer = read_memory_16(program_counter);
            break;
        case 0x32:
            // LD (HL-), A: Store the contents of register A into the memory location specified by register pair HL,
            // and simultaneously decrement the contents of HL.
            write_memory(get_dual_register(HL_REGISTER), get_register(A_REGISTER));
            sub_dual_register(HL_REGISTER, 1);
            break;
        case 0x33:
            // INC SP: Increment the contents of register pair SP by 1.
            stack_pointer++;
            break;
        case 0x34:
            // INC (HL): Increment the contents of memory specified by register pair HL by 1.
            address = get_dual_register(HL_REGISTER);
            value = read_memory(address) + 1;
            write_memory(address, value);
            break;
        case 0x35:
            // DEC (HL): Decrement the contents of memory specified by register pair HL by 1.
            address = get_dual_register(HL_REGISTER);
            value = read_memory(address) - 1;
            write_memory(address, value);
            break;
        case 0x36:
            // LD (HL), d8: Store the contents of 8-bit immediate operand d8 in the memory location specified by register pair HL.
            program_counter++;
            write_memory(get_dual_register(HL_REGISTER), read_memory(program_counter));
            break;
        case 0x37:
            // SCF: Set Carry flag
            set_flag(FLAG_CARRY, 1);
            break;
        case 0x38:
            // JR C, s8: If the CY flag is 1, jump s8 steps from the current address stored in the program counter (PC).
            // If not, the instruction following the current JP instruction is executed (as usual).
            program_counter++;
            if (get_flag(FLAG_CARRY) == 1)
                program_counter += read_memory(program_counter);
            break;
        case 0x39:
            // ADD HL, SP: Add the contents of register pair SP to the contents of register pair HL, and store the results in register pair HL.
            add_dual_register(HL_REGISTER, stack_pointer);
            break;

        case 0x3A:
            // LD A, (HL-): Load the contents of memory specified by register pair HL into register A, and simultaneously decrement the contents of HL.
            set_register_rr(A_REGISTER, HL_REGISTER);
            sub_dual_register(HL_REGISTER, 1);
            break;
        case 0x3B:
            // DEC SP
            stack_pointer--;
            break;
        case 0x3C:
            // INC L: Increase contents of L Register by 1
            increase_register(L_REGISTER);
            break;
        case 0x3D:
            // DEC L: Decrease contents of L register by 1
            decrease_register(L_REGISTER);
            break;
        case 0x3E:
            // LD L, d8: Load the 8-bit immediate operand d8 into register L.
            program_counter++;
            set_register(L_REGISTER, read_memory(program_counter));
            break;
        case 0x3F:
            // CCF: Flip the carry flag CY.
            set_flag(FLAG_CARRY, get_flag(FLAG_CARRY) ^ 1);
            break;
        }
    }
    else if (code == 0x76)
    {
        halt();
    }
    else if (code < 0x80)
    {
        ld_block(code);
    }
    else if (code < 0xC0)
    {
        a_operation_block(code);
    }
    else if (code < 0xFF)
    {
        switch (code)
        {
        case 0xC0:
            // RET NZ
            if (get_flag(FLAG_ZERO) == 0)
            {
                return_from_stack();
            }
            break;
        case 0xC1:
        case 0xD1:
        case 0xE1:
        case 0xF1:
            // POP
            break;
        case 0xC2:
            // JP NZ, a16:
            break;
        case 0xC3:
            // JP a16
            break;
        case 0xC4:
            // CALL NZ, a16
            break;
        case 0xC5:
        case 0xD5:
        case 0xE5:
        case 0xF5:
            // PUSH
            push_block(code);
            break;
        case 0xC6:
            // ADD A, d8
            break;
        case 0xC7:
        case 0xD7:
        case 0xE7:
        case 0xF7:
            // RST
            break;
        case 0xC8:
            // RET Z
            if (get_flag(FLAG_ZERO) == 1)
            {
                return_from_stack();
            }
            break;
        case 0xC9:
            // RET
            return_from_stack();
            break;
        case 0xCA:
            // JP Z, a16
            break;
        case 0xCB:
        case 0xDB:
        case 0xEB:
            // No instruction
            break;
        case 0xCC:
            // CALL Z, a16
            break;
        case 0xCD:
            // CALL a16
            break;
        case 0xCE:
            // ADC A, d8
            program_counter++;
            add_register_carry(read_memory(program_counter));
            break;
        case 0xCF:
        case 0xDF:
        case 0xEF:
        case 0xFF:
            // RST
            break;
        case 0xD0:
            // RET NC
            if (get_flag(FLAG_CARRY) == 0)
            {
                return_from_stack();
            }
            break;
        case 0xD2:
            // JP NC, a16
            if (get_flag(FLAG_CARRY) == 0)
                program_counter = read_memory_16(program_counter + 1);
            else
                program_counter += 2;
            break;
        case 0xD3:
            // No instruction
            break;
        case 0xD4:
            // CALL NC, a16
            break;
        case 0xD6:
            // SUB d8
            program_counter++;
            sub_register(read_memory(program_counter));
            break;
        case 0xD8:
            // RET C
            if (get_flag(FLAG_CARRY) == 1)
            {
                return_from_stack();
            }
            break;
        case 0xD9:
            // RETI
            return_from_stack();
            set_IME(true);
            break;
        case 0xDA:
            // JP C, a16
            if (get_flag(FLAG_CARRY))
            {
                program_counter++;
                program_counter = read_memory_16(program_counter);
            }
            else
                program_counter += 2;
            break;
        case 0xDC:
            // CALL C, a16
            if (get_flag(FLAG_ZERO))
            {
                write_memory_16(stack_pointer, program_counter + 2);
                program_counter++;
                program_counter = read_memory_16(program_counter);
            }
            break;
        case 0xDD:
            // No instruction
            break;
        case 0xDE:
            // SBC A, d8
            program_counter++;
            sub_register_carry(read_memory(program_counter));
            break;
        case 0xE0:
            // LD (a8), A

            break;
        case 0xE2:
            // LD (C), A
            break;
        case 0xE3:
        case 0xE4:
            // No instruction
            break;
        case 0xE6:
            // AND d8
            program_counter++;
            and_register(read_memory(program_counter));
            break;
        case 0xE8:
            // ADD SP, s8
            program_counter++;
            stack_pointer += (int8_t)program_counter;
            break;
        case 0xE9:
            // JP HL
            program_counter = get_dual_register(HL_REGISTER);
            break;
        case 0xEA:
            // LD (a16), A
            break;
        case 0xEE:
            // XOR d8
            program_counter++;
            xor_register(read_memory(program_counter));
            break;
        case 0xF0:
            // LD A, (a8)

            break;
        case 0xF2:
            // LD A, (C)
            break;
        case 0xF3:
            // DI
            break;
        case 0xF6:
            // OR d8
            program_counter++;
            or_register(read_memory(program_counter));
            break;
        case 0xF8:
            // LD HL, SP+s8
            break;
        case 0xF9:
            // LD SP, HL
            stack_pointer = get_dual_register(HL_REGISTER);
            break;
        case 0xFA:
            // LD A, (a16)
            program_counter++;
            uint16_t address = read_memory_16(program_counter);
            set_register(A_REGISTER, read_memory(address));
            break;
        case 0xFB:
            // EI
            break;
        default:
            perror("Instruction is not supported!");
            break;
        }
    }

    program_counter++;
}