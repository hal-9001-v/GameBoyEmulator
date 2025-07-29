#include "tests.h"
#include "basic_instructions.h"
#include "cpu.h"
#include "memory_reader.h"

void setUp(void)
{
    // set stuff up here
}

void tearDown(void)
{
    // clean stuff up here
}

void basic_operation_tests(void)
{
    combine(0xF0, 0x0F);
    TEST_ASSERT_EQUAL_UINT8(combine(0x0A, 0xF0), 0xF00A);

    from_combination(0xA0B1);
    TEST_ASSERT_EQUAL_UINT8(combination_buffer[0], 0xB1);
    TEST_ASSERT_EQUAL_UINT8(combination_buffer[1], 0xA0);
}

void memory_tests(void)
{
    write_memory(WORK_RAM_1_START, 0xEA);
    TEST_ASSERT_EQUAL_UINT8(read_memory(WORK_RAM_1_START), 0xEA);

    write_memory(WORK_RAM_2_START + 2, 0x01);
    TEST_ASSERT_EQUAL_UINT8(read_memory(WORK_RAM_2_START + 2), 0x01);
}

void bit_operations_tests(void)
{
    uint16_t a = 0xFFFF;
    TEST_ASSERT_EQUAL_UINT16(0xFFFE, set_bit_16(a, 0, 0));
    TEST_ASSERT_EQUAL_UINT16(0xFFEF, set_bit_16(a, 4, 0));
    TEST_ASSERT_EQUAL_UINT16(0xFEFF, set_bit_16(a, 8, 0));
    TEST_ASSERT_EQUAL_UINT16(0xEFFF, set_bit_16(a, 12, 0));
    TEST_ASSERT_EQUAL_UINT16(0x7FFF, set_bit_16(a, 15, 0));

    uint8_t b = 0xFF;
    TEST_ASSERT_EQUAL_UINT8(0xFE, set_bit(a, 0, 0));
    TEST_ASSERT_EQUAL_UINT8(0xF7, set_bit(a, 3, 0));
    TEST_ASSERT_EQUAL_UINT8(0xEF, set_bit(a, 4, 0));
    TEST_ASSERT_EQUAL_UINT8(0x7F, set_bit(a, 7, 0));

    TEST_ASSERT_EQUAL_UINT16(0xFFFE, rotate_left_16(0x7FFF));
    TEST_ASSERT_EQUAL_UINT16(0xFFFD, rotate_left_16(0xFFFE));

    TEST_ASSERT_EQUAL_UINT16(0xBFFF, rotate_right_16(0x7FFF));
    TEST_ASSERT_EQUAL_UINT16(0x7FFF, rotate_right_16(0xFFFE));

    TEST_ASSERT_EQUAL_UINT8(0xFE, rotate_left(0x7F));
    TEST_ASSERT_EQUAL_UINT8(0xFD, rotate_left(0xFE));

    TEST_ASSERT_EQUAL_UINT8(0xBF, rotate_right(0x7F));
    TEST_ASSERT_EQUAL_UINT8(0x7F, rotate_right(0xFE));

    TEST_ASSERT_EQUAL_UINT16(0x0907, combine(0x07, 0x09));
    TEST_ASSERT_EQUAL_UINT16(0xA210, combine(0x10, 0xA2));

    from_combination(0xABCD);
    uint8_t expected[] = {0xCD, 0xAB};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, combination_buffer, 2);
    from_combination(0x3277);
    expected[0] = 0x77;
    expected[1] = 0x32;
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, combination_buffer, 2);
}

void check_dual_register(uint8_t index)
{
    printf("Testing dual register %i \n", index);
    set_dual_register(index, 0xA001);
    sub_dual_register(index, 0x9000);
    TEST_ASSERT_EQUAL_UINT16(0x1001, get_dual_register(index));

    set_dual_register(index, 0x8123);
    add_dual_register(index, 0x00D);
    TEST_ASSERT_EQUAL_UINT16(0x8130, get_dual_register(index));

    set_dual_register(index, 0x8080);
    rotate_register_left(index);
    TEST_ASSERT_EQUAL_UINT16(0x0101, get_dual_register(index));

    set_dual_register(index, 0x8001);
    rotate_register_right(index);
    TEST_ASSERT_EQUAL_UINT16(0xC000, get_dual_register(index));

    set_dual_register(index, 0x8001);
    rotate_register_right_carry(index);
    TEST_ASSERT_EQUAL_UINT16(0xC000, get_dual_register(index));
    TEST_ASSERT_EQUAL_UINT8(1, get_flag(FLAG_CARRY));

    set_dual_register(index, 0x6002);
    rotate_register_right_carry(index);
    TEST_ASSERT_EQUAL_UINT16(0x3001, get_dual_register(index));
    TEST_ASSERT_EQUAL_UINT8(0, get_flag(FLAG_CARRY));

    set_dual_register(index, 0x4010);
    rotate_register_left_carry(index);
    TEST_ASSERT_EQUAL_UINT16(0x8020, get_dual_register(index));
    TEST_ASSERT_EQUAL_UINT8(0, get_flag(FLAG_CARRY));

    set_dual_register(index, 0x9080);
    rotate_register_left_carry(index);
    TEST_ASSERT_EQUAL_UINT16(0x2101, get_dual_register(index));
    TEST_ASSERT_EQUAL_UINT8(1, get_flag(FLAG_CARRY));
}

void cpu_register_operations_test(void)
{
    set_register(E_REGISTER, 0xA1);
    decrease_register(E_REGISTER);
    TEST_ASSERT_EQUAL_UINT16(0xA0, get_register(E_REGISTER));

    set_register(D_REGISTER, 0x02);
    increase_register(D_REGISTER);
    TEST_ASSERT_EQUAL_UINT8(0x03, get_register(D_REGISTER));

    check_dual_register(HL_REGISTER);
    check_dual_register(BC_REGISTER);
    check_dual_register(DE_REGISTER);

    set_register(A_REGISTER, 0x81);
    rotate_register_left(A_REGISTER);
    TEST_ASSERT_EQUAL_UINT8(0x03, get_register(A_REGISTER));

    set_register(A_REGISTER, 0x81);
    rotate_register_left_carry(A_REGISTER);
    TEST_ASSERT_EQUAL_UINT8(0x03, get_register(A_REGISTER));
    TEST_ASSERT_EQUAL_UINT8(get_flag(FLAG_CARRY), 0x01);

    set_register(A_REGISTER, 0x81);
    rotate_register_right(A_REGISTER);
    TEST_ASSERT_EQUAL_UINT8(0xC0, get_register(A_REGISTER));

    set_register(A_REGISTER, 0x81);
    rotate_register_right_carry(A_REGISTER);
    TEST_ASSERT_EQUAL_UINT8(0xC0, get_register(A_REGISTER));
    TEST_ASSERT_EQUAL_UINT8(get_flag(FLAG_CARRY), 0x01);
}

void mask_operations_test(void)
{
    set_register(A_REGISTER, 0xFF);
    and_register(0xF0);
    TEST_ASSERT_EQUAL_UINT8(get_register(A_REGISTER), 0xF0);

    set_register(A_REGISTER, 0xFF);
    set_dual_register(HL_REGISTER, WORK_RAM_1_START);
    set_register(HL_REGISTER, 0xF0);
    and_register_rr(HL_REGISTER);
    TEST_ASSERT_EQUAL_UINT8(get_register(A_REGISTER), 0xF0);

    set_register(A_REGISTER, 0x0F);
    xor_register(0xFF);
    TEST_ASSERT_EQUAL_UINT8(get_register(A_REGISTER), 0xF0);

    set_register(A_REGISTER, 0xFF);
    xor_register(0xFF);
    TEST_ASSERT_NOT_EQUAL_UINT8(get_register(A_REGISTER), 0xF0);

    set_register(A_REGISTER, 0x0F);
    or_register(0xF0);
    TEST_ASSERT_EQUAL_UINT8(get_register(A_REGISTER), 0xFF);
}

int start_tests()
{
    UNITY_BEGIN();
    RUN_TEST(basic_operation_tests);
    RUN_TEST(memory_tests);
    RUN_TEST(bit_operations_tests);
    RUN_TEST(cpu_register_operations_test);
    RUN_TEST(mask_operations_test);

    return UNITY_END();
}
