#include "tests.h"
#include "basic_instructions.h"
#include "cpu.h"

void setUp(void)
{
    // set stuff up here
}

void tearDown(void)
{
    // clean stuff up here
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

    check_dual_register(HL_REGISTER);
    check_dual_register(BC_REGISTER);
    check_dual_register(DE_REGISTER);
}

int start_tests()
{
    UNITY_BEGIN();
    RUN_TEST(bit_operations_tests);
    RUN_TEST(cpu_register_operations_test);
    return UNITY_END();
}
