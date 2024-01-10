#include "vm_definitions.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define STACK_SIZE  256
#define MEMORY_SIZE 1024
#define MAX_PROGRAM_SIZE 1000 // Max instructions to read from file
							 
Registers registers = {0};

#define pc registers.PC
#define sp registers.SP

#define MOV_FLAG 0x01 // Rightmost bit represents mov flag - tells mov if the value is given (1) or pulled from the stack (0)

int stack[STACK_SIZE];
int memory[MEMORY_SIZE];

int program[MAX_PROGRAM_SIZE];

// These are used to map the string instructions from program file to enum values for handler funcitons
const char* instructionStrings[] = {"PSH", "POP", "ADD", "SUB", "MOV", "ALC", "FRE", "ST", "LD", "MSG", "HLT"};
const int instructionCount = 11;

bool running = true;

// Table of function pointers for handling instructions
typedef void (*InstructionHandler)();
InstructionHandler handlers[] =
{
	handle_PSH,
	handle_POP,
	handle_ADD,
	handle_SUB,
	handle_MOV,
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
		if (strcmp(token, "MOV") == 0)
		{
			program[i] = mapStringToEnum(token); // Put the MOV instruction in the program array
			helper_MOV(file, &i);
		}
		else if (isdigit(token[0]))
		{
			program[i] = atoi(token); // Integers are allowed as values for certain instructions.
									  // TODO: CURRENTLY NO ERROR HANDLING TO CHECK IF THE INSTRUCTION ALLOWS INTEGERS
									  // FIXME: CHECK THE INSTRUCTION IF THE FOLLOWING TOKEN IS A DIGIT
									  // 		could do this with the flags?
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
	for (int i = 0; i < instructionCount; ++i)
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
	int count = fscanf(file, "%d", &val);
	if (count == 1)
	{
		program[*i+1] = val;
		count = fscanf(file, " %c", &dstReg);
		if (count == 1)
		{
			// A second argument is found, indicating the destination register.
			program[*i+2] = dstReg;
			setFlag(&registers.flags, MOV_FLAG);
			*i+=2;
		}
		else
		{
			// Only one argument found, clear flag.
			clearFlag(&registers.flags, MOV_FLAG);
		}
	}
}

void handle_MSG()
{
	printf("top of the stack: %d\n", stack[sp]);
}

void handle_HLT()
{
	printf("Executing HLT\n");
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
		int sum = stack[sp-1] + stack[sp]; // Adds the top two values on the stack together
		sp--;                              // decrement stack pointer
		sp++;                              // Allocate space on the stack
		stack[sp] = sum;                   // Push sum onto stack
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
		int diff = stack[sp-1] - stack[sp]; // Subtracts the 2nd stack value from the top stack value
		sp-=2;
		sp++;
		stack[sp] = diff;
	}
	else
	{
		fprintf(stderr, "ERROR: Stack underflow during SUB operation: %d\n", pc);
		running = false;
	}
}

void handle_MOV()
{
	if (sp >= 1)
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

		switch (dstReg)
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
				fprintf(stderr, "ERROR: Unknown register \"%d\"\n", dstReg);
				break;
		}
		*dstPtr = value; // Set the register to hold the given value;
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
