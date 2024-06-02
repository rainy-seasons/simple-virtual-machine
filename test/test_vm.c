#include "test_vm.h"

Registers test_registers;

void setUp(void)
{
	test_registers = (Registers){0}; // Initialize all registers to 0
}

void tearDown(void)
{
}

void test_setFlag(void)
{
	setFlag(&test_registers.flags, CMP_FLAG);         // Set the flag
	bool flagVal = (test_registers.flags & CMP_FLAG); // Check the flag value
	TEST_ASSERT_EQUAL_UINT32(1, flagVal);             // Assert that it is set
}

void test_clearFlag(void)
{
	test_registers.flags |= CMP_GT_FLAG;                               // Set the flag
	if (!isFlagSet(test_registers.flags, CMP_GT_FLAG))
		printf("ERROR: Flag was not set before testing clearFlag()");
	clearFlag(&test_registers.flags, CMP_GT_FLAG);                     // Clear it
	TEST_ASSERT_EQUAL_UINT32(0, (test_registers.flags & CMP_GT_FLAG)); // Make sure it's cleared
}

void test_isFlagSet(void)
{
	test_registers.flags |= CMP_FLAG; // Set the CMP_FLAG
	if (!(test_registers.flags & CMP_FLAG))
		printf("ERROR: Flag was not set before testing isFlagSet()");
	TEST_ASSERT_EQUAL_UINT32(1, (isFlagSet(test_registers.flags, CMP_FLAG)));
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

int main(void)
{
	UNITY_BEGIN();
	RUN_TEST(test_setFlag);
	RUN_TEST(test_clearFlag);
	//RUN_TEST(test_getRegister);
	RUN_TEST(test_isFlagSet);
	return UNITY_END();
}
