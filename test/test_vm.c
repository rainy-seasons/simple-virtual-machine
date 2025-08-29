#include "test_vm.h"

Registers test_registers;

void setUp(void)
{
	test_registers = (Registers){0}; // Initialize all registers to 0
}

void tearDown(void)
{
}

int main(void)
{
	UNITY_BEGIN();
	RUN_TEST(test_setFlag);
	RUN_TEST(test_clearFlag);
	//RUN_TEST(test_getRegister);
	RUN_TEST(test_isFlagSet);
	RUN_TEST(test_handle_PSH);
	return UNITY_END();

void test_setFlag(void)
{
	setFlag(&test_registers.flags, CMP_FLAG);
	TEST_ASSERT_TRUE(test_registers.flags & CMP_FLAG);
}

void test_clearFlag(void)
{
	test_registers.flags |= CMP_GT_FLAG;                               // Set the flag
	TEST_ASSERT_TRUE(isFlagSet(test_registers.flags, CMP_GT_FLAG));    // verify precondition
	clearFlag(&test_registers.flags, CMP_GT_FLAG);
	TEST_ASSERT_FALSE(test_registers.flags & CMP_GT_FLAG);
}

void test_isFlagSet(void)
{
	test_registers.flags |= CMP_FLAG; // Set the CMP_FLAG
	TEST_ASSERT_TRUE(isFlagSet(test_registers.flags, CMP_FLAG));
}

/* TODO */
/* This won't work since getRegister() inherently uses the other registers declared in vm_definitions.c */
void test_getRegister(void)
{
	//registers.A = 10;
	int* ptr = getRegister('A');
	if (*ptr != 10)
		printf("ERROR: Register was not set before testing getRegister()");
	TEST_ASSERT_NOT_NULL(ptr); // Assert that the returned pointer is not NULL
    TEST_ASSERT_EQUAL_INT(10, *ptr);
}

void test_handle_PSH()
{
	TEST_IGNORE_MESSAGE("TODO: Not yet implemented.");
}
