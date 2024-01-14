#include "vm_definitions.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define STACK_SIZE  1024
#define MEMORY_SIZE 1024
#define MAX_PROGRAM_SIZE 1000 // Max instructions to read from file
							 
Registers registers = {0};

#define pc registers.PC
#define sp registers.SP

#define MOV_FLAG   (1 << 0) // Rightmost bit represents mov flag - tells mov if the value is given (1) or pulled from the stack (0)
#define CMP_FLAG   (1 << 1) // Second rightmost bit represents the CMP flag - records the result of a comparison. (1) if same, (0) if different
#define CMP_R_FLAG (1 << 2) // (1) When CMP is taking two registers as arguments. (0) When taking one register and one explicit value

int stack[STACK_SIZE];
//int memory[MEMORY_SIZE]; // TODO: Currently unused.

int program[MAX_PROGRAM_SIZE];

// These are used to map the string instructions from program file to enum values for handler funcitons
const char* instructionStrings[] = {"PSH", "POP", "POR", "ADD", "SUB", "MUL", "DIV", "MOV", "CMP", "JMP", "JIE", "JNE", "ALC", "FRE", "ST", "LD", "MSG", "HLT", "INSTRUCTION_COUNT"};

bool running = true;

// Table of function pointers for handling instructions
typedef void (*InstructionHandler)();
InstructionHandler handlers[] =
{
	handle_PSH,
	handle_POP,
	handle_POR,
	handle_ADD,
	handle_SUB,
	handle_MUL,
	handle_DIV,
	handle_MOV,
	handle_CMP,
	handle_JMP,
	handle_JIE,
	handle_JNE,
	handle_ALC,
	handle_FRE,
	handle_ST,
	handle_LD,
	handle_MSG,
	handle_HLT,
};

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s <program_file>\n", argv[0]);
		return EXIT_FAILURE;
	}
	registers.flags = 0;
	loadProgram(argv[1]);
	while (running)
	{
		evaluate();
	}
	return 0;
}

void loadProgram(const char* filename)
{
	FILE* file = fopen(filename, "r");

	if (file == NULL)
	{
		fprintf(stderr, "ERROR: Unable to open file: %s", filename);
		exit(EXIT_FAILURE);
	}
	int i = 0;
	char token[4]; // Max length of instruction should be 3
	while (fscanf(file, "%3s", &token) != EOF)
	{
		//TODO: Clean this up and do it a better way.
		if (strcmp(token, "MOV") == 0)
		{
			program[i] = mapStringToEnum(token); // Put the MOV instruction in the program array
			helper_MOV(file, &i);
		}
		else if (strcmp(token, "POR") == 0)
		{
			program[i] = mapStringToEnum(token);
			helper_POR(file, &i);
		}
		else if (strcmp(token, "CMP") == 0)
		{
			program[i] = mapStringToEnum(token);
			helper_CMP(file, &i);
		}
		else if (isdigit(token[0]))
		{
			program[i] = atoi(token); // Integers are allowed as values for certain instructions.
		}
		else
		{
			program[i] = mapStringToEnum(token);
		}
		i++;
	}
	fclose(file);
}

// Maps the input string for an instruction to its respective enum value
InstructionSet mapStringToEnum(const char* str)
{
	for (int i = 0; i < INSTRUCTION_COUNT; ++i)
	{
		if (strcmp(str, instructionStrings[i]) == 0)
		{
			return (InstructionSet)i;
		}
	}
	fprintf(stderr, "ERROR: Unknown instruction \"%s\"\n", str);
	return HLT;
}

// Sets a flag
void setFlag(int* flags, int flag)
{
	*flags |= flag;
}

// Clears a flag
void clearFlag(int* flags, int flag)
{
	*flags &= ~flag;
}

// Checks if a flag is set
int isFlagSet(int flags, int flag)
{
	return (flags & flag) != 0;
}

/* HELPER FUNCTION TO RETURN A REGISTER GIVEN ITS STRING REPRESENTATION */
int* getRegister(char reg)
{
	int* dstPtr = NULL;
	switch (reg)
	{
		case 'A':
			dstPtr = &registers.A;
			break;
		case 'B':
			dstPtr = &registers.B;
			break;
		case 'C':
			dstPtr = &registers.C;
			break;
		case 'D':
			dstPtr = &registers.D;
			break;
		case 'E':
			dstPtr = &registers.E;
			break;
		default:
			fprintf(stderr, "ERROR: Unknown register \"%d\"\n", reg);
			break;
	}
	return dstPtr;
}

void evaluate()
{
	int instruction = program[pc];
    handlers[instruction]();
	pc++;
}

/* HELPER FUNCTION TO HANDLE SETTING UP MOV OPERATIONS */
void helper_MOV(FILE* file, int* i)
{
	int val; 
	char dstReg;
	if (fscanf(file, " %d", &val) == 1)          // Checking if the argument following the MOV instruction is an int.
	{
		program[*i+1] = val;                     // Move the value into the program array
		if (fscanf(file, " %c", &dstReg) == 1)   // Checking for destination register.
		{
			program[*i+2] = dstReg;              // Move destination register into program array.
			setFlag(&registers.flags, MOV_FLAG); // Set the MOV_FLAG - indicating the value is given explicitly
			*i+=2;                               // Increment to skip over the value and register.
		}
	}
	else if (fscanf(file, "%c", &dstReg) == 1)   // Only the destination register is given - value is taken from the stack
	{
		program[*i+1] = dstReg;					 // Push the destination register into the program array
		clearFlag(&registers.flags, MOV_FLAG);   // Clear the MOV_FLAG indicating implicit value retrieval
		*i+=1; 									 // Increment to skip over the register
	}
}

/* HELPER FUNCTION TO HANDLE SETTING UP POR OPERATIONS */
void helper_POR(FILE* file, int* i)
{
	char reg;
	if (fscanf(file, " %c", &reg) == 1) // Check if the register is present
	{
		program[*i+1] = reg; // Add register to program array
		*i+=1;
	}
}

/* HELPER FUNCTION TO HANDLE SETTING UP CMP OPERATIONS */
void helper_CMP(FILE* file, int* i)
{
	char reg, reg2;
	int arg2;
	if (fscanf(file, " %c", &reg) == 1) // First argument should always be a register
	{
		program[*i+1] = reg;
		if (fscanf(file, " %c", &reg2) == 1) // Check if 2nd arg is also a register (char)
		{
			program[*i+2] = reg2;
			setFlag(&registers.flags, CMP_R_FLAG); // Set flag so the handler function knows to expect two registers
		}
		else if (fscanf(file, " %d", &arg2) == 1) // Check if 2nd arg is an explicit int
		{
			program[*i+2] = arg2;
			clearFlag(&registers.flags, CMP_R_FLAG); // Clear flag so handler function knows to expect one register and one value
		}
	}
	else
	{
		fprintf(stderr, "ERROR: Inavlid usage of CMP instruction: %d - CMP {register} {register} or CMP {register} {value}", pc);
	}
	*i+=2; // Skip the arguments since they've been handled in this function
}

void handle_MSG()
{
	printf("top of the stack: %d\n", stack[sp]);
}

void handle_HLT()
{
	printf("HLT: HALTING EXECUTION\n");
	running = false;
}

void handle_PSH()
{
	sp++;                    // Allocate space on the stack
	pc++;                    // Increment the program counter, to retrieve the value
	stack[sp] = program[pc]; // push the value following the instruction onto the stack
	printf("Pushed %d\n", stack[sp]);
}

void handle_POP()
{
	if (sp >= 0)
	{
		registers.C = stack[sp--]; // Store top stack value and pop it
	}
	else
	{
		fprintf(stderr, "ERROR: Stack underflow during POP operation: %d\n", pc);
		running = false;
	}
}

void handle_ADD()
{
	if (sp >= 1)
	{
		int val1 = stack[sp];
		int val2 = stack[sp-1];
		int sum = val1 + val2; // Adds the top two values on the stack together
		sp--;                              // decrement stack pointer
		sp++;                              // Allocate space on the stack
		stack[sp] = sum;                   // Push sum onto stack
		printf("ADD: %d + %d = %d\n", val1, val2, sum);
	}
	else
	{
		fprintf(stderr, "ERROR: Stack underflow during ADD operation: %d\n", pc);
		running = false;
	}
}

void handle_SUB()
{
	if (sp >= 1)
	{
		int val1 = stack[sp-1];
		int val2 = stack[sp];
		int diff = val1 - val2; // Subtracts the 2nd stack value from the top stack value
		sp-=2;
		sp++;
		stack[sp] = diff;
		printf("SUB: %d - %d = %d\n", val1, val2, diff);
	}
	else
	{
		fprintf(stderr, "ERROR: Stack underflow during SUB operation: %d\n", pc);
		running = false;
	}
}

void handle_MUL()
{
	if (sp >= 0)
	{
		int val1 = stack[sp-1];
		int val2 = stack[sp];
		int product = val1 * val2;
		sp-=2;
		sp++;
		stack[sp] = product;
		printf("MUL: %d * %d = %d\n", val1, val2, product);
	}
	else
	{
		fprintf(stderr, "ERROR: Stack underflow during MUL operation: %d\n", pc);
	}
}

void handle_DIV()
{
	if (sp >= 0)
	{
		int val1 = stack[sp-1];
		int val2 = stack[sp];
		int quotient = val1 / val2;
		sp-=2;
		sp++;
		stack[sp] = quotient;
		printf("MUL: %d / %d = %d\n", val1, val2, quotient);
	}
	else
	{
		fprintf(stderr, "ERROR: Stack underflow during MUL operation: %d\n", pc);
	}
}

void handle_MOV()
{
	if (sp >= 0)
	{
		int  value;
		int  dstReg;
		int* dstPtr;

		// A value is given explicitly
		if (isFlagSet(registers.flags, MOV_FLAG))
		{
			value = program[++pc];
			dstReg = program[++pc];
		}
		// The value is not given explicitly so it's pulled from the stack
		else
		{
			dstReg = program[++pc];
			value = stack[sp--];
		}

		dstPtr = getRegister((char)dstReg);
		*dstPtr = value; // Set the register to hold the given value;
						 
		printf("MOV: Register %c now contains value %d\n", dstReg, value);
	}
}

void handle_CMP()
{
	if (sp >= 0)
	{
		int  reg1 = program[++pc];
		int  reg2 = program[++pc];
		int  val1, val2;
		int* pReg;

		// val1 is always going to be a register so just grab the value in that register ahead of time
		pReg = getRegister((char)reg1);
		val1 = *pReg;

		if (isFlagSet(registers.flags, CMP_R_FLAG))
		{
			pReg = getRegister((char)reg2);
			val2 = *pReg;
		}
		else
		{
			val2 = reg2;
		}

		if (val1 == val2)
		{
			printf("CMP: %d and %d are equal.\n", val1, val2);
			setFlag(&registers.flags, CMP_FLAG);
		}
		else
		{
			printf("CMP: %d and %d are NOT equal.\n", val1, val2);
			clearFlag(&registers.flags, CMP_FLAG);
		}
	}
	else
	{
		fprintf(stderr, "ERROR: Stack underflow during CMP operation: %d", pc);
	}
}

void handle_JMP()
{
	printf("JMP: NOT YET IMPLEMENTED\n");
}

void handle_JIE()
{
	printf("JIE: NOT YET IMPLEMENTED\n");
}

void handle_JNE()
{
	printf("JNE: NOT YET IMPLEMENTED\n");
}

void handle_POR()
{
	if (sp >= 0)
	{
		int reg;
		int* pReg;
		reg = program[++pc];     // Get the register from the program array
		pReg = getRegister((char)reg); // Get a pointer to the register
		stack[++sp] = *pReg;     // Push the value from the register onto the stack
		printf("POR: Popped value %d from register %c\n", *pReg, reg);
		*pReg = 0;               // Clear the register
	}
}

void handle_ALC()
{
	size_t size = program[++pc];                                                  // Gets the size value that follows the ALLOC instruction. How much memory to allocate.
	int* ptr = malloc(size * sizeof(int));                                        // Allocate some memory and store the address in ptr
	if (ptr == 0)
	{
		fprintf(stderr, "ERROR: Failed to allocate memory.\n");
		running = false;
	}
	else
	{
		registers.F = ptr;                                                        // Pushes the address of the allocated memory into the F register
	}
}

void handle_FRE()
{
	int* ptr = registers.F;
	free((void*)ptr);                                                             // Free the memory
	registers.F = NULL;
	printf("FREED MEMORY AT ADDRESS: 0x%p\n", registers.F);
}

void handle_ST()
{
	if (sp >= 0)
	{
		int value = stack[sp--];                                                  // Take value from top of stack and pop it

		int* address = registers.F;                                               // Retrieve the address that was allocated and stored in F register

		if (address == NULL)
		{
			fprintf(stderr, "ERROR: Attempting to store value in NULL pointer.");
			running = false;
			return;
		}
		*registers.F = value;                                                     // Dereference pointer and set value to the value popped off the stack
		printf("ST: Stored value: %d\n", value);
	}
	else
	{
		fprintf(stderr, "ERROR: Stack underflow during ST operation: %d\n", pc);
		running = false;
	}
}

void handle_LD()
{
	if (sp >= 0)
	{
		int* address = registers.F;                                               // Get the pointer from F register

		int value = *(int*)address;                                               // Get the value held at the address

		stack[++sp] = value;                                                      // Push loaded value onto the stack
	}
	else
	{
		fprintf(stderr, "ERROR: Stack underflow during LD operation: %d\n", pc);
		printf("Entered else block in LD\n");
	}
}
