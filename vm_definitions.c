#include "vm_definitions.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

bool running = true;

Registers registers = {0};
#define pc (registers.PC)
#define sp (registers.SP)

int stack[STACK_SIZE];
int program[MAX_PROGRAM_SIZE];

// These are used to map the string instructions from program file to enum values for handler funcitons
const char* instructionStrings[] = {"PSH", "POP", "POR", "ADD", "SUB", "MUL", "DIV", "MOV", "CMP", "JMP", "JIE", "JNE", "JGT", "JLT", "ALC", "FRE", "ST", "LD", "MSG", "HLT", "INSTRUCTION_COUNT"};

// These are used to keep track of labels for jumps
KeyValueMap LabelMap;
char* labels[100];

// TODO: These are //temporory. Just to keep track of arg count for each instruction of its kind.
ARG_INFO MOV_INFO;
ARG_INFO CMP_INFO;

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
	handle_JGT,
	handle_JLT,
	handle_ALC,
	handle_FRE,
	handle_ST,
	handle_LD,
	handle_MSG,
	handle_HLT,
};

void init_type_vars()
{
	LabelMap.size  = 0;
	MOV_INFO.index = 0;
	MOV_INFO.size  = 0;
	CMP_INFO.index = 0;
	CMP_INFO.size  = 0;
}

void evaluate()
{
	int instruction = program[pc];
	handlers[instruction](); // Calls the respective function from the InstructionHandler 
	pc++;
}
void loadProgram(const char* filename)
{
	registers.flags = 0;
	FILE* file = fopen(filename, "r");

	if (file == NULL) 
	{
		fprintf(stderr, "ERROR: Unable to open file: %s", filename);
		exit(EXIT_FAILURE);
	}
	int   val;
	int   i = 0;
	char  line[256]; // line buffer
	char* token;
	char* arg;
	while (fgets(line, sizeof(line), file) != NULL) // Read line by line
	{
		token = strtok(line, " ");                  // get the instruction
		token[strcspn(token, "\r\n")] = '\0';       // Trim newline characters off the token

		if (token[0] == ';') // Skip comments - token system automatically ignores inline comments
		{
			continue;
		}

		if (token)
		{
			/* Token is a label */
			if (strcmp(token, "label:") == 0)
			{
				char* label = strtok(NULL, " ");
				label[strcspn(label, "\r\n")] = '\0';
				addToMap(&LabelMap, label, i);
				continue;
			}
			program[i] = mapStringToEnum(token); // Add instruction enum value to program array
			if (strcmp(token, "MOV") == 0)
			{
				helper_MOV(&i);
			}
			else if (strcmp(token, "POR") == 0)
			{
				helper_POR(&i);
			}
			else if (strcmp(token, "CMP") == 0)
			{
				helper_CMP(&i);
			}
			else if (strcmp(token, "JMP") == 0 || strcmp(token, "JIE") == 0 || strcmp(token, "JNE") == 0 || strcmp (token, "JGT") == 0 || strcmp(token, "JLT") == 0)
			{
				helper_JMP(&i);
			}
			else
			{
				arg = strtok(NULL, " "); // Get the argument
				if (arg)
				{
					val = atoi(arg);     // cast arg to int -- only thing here should be integers passed to instructions like PSH. Register args are handled in helper functions.
					program[++i] = val;  // add to program array
				}
			}
		}
		i++;
	}
	fclose(file);
}

// Maps the input string for an instruction to its respective enum value (InstructionSet)
InstructionSet mapStringToEnum(const char* str)
{
	for (int i = 0; i < INSTRUCTION_COUNT; ++i)
	{
		if (strcmp(str, instructionStrings[i]) == 0)
		{
			return (InstructionSet)i; // Gets the enum value
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

/* Adds label to map */
void addToMap(KeyValueMap* map, const char* key, int value)
{
	if (map->size < 100)
	{
		strcpy(map->pairs[map->size].key, key);
		map->pairs[map->size].value = value;
		map->size++;
	}
	else
	{
		fprintf(stderr, "ERROR: Map is full. Cannot add more labels.\n");
	}
}

/* Gets label value from label map using the label name */
int getValue(KeyValueMap* map, const char* key)
{
	for (int i = 0; i < map->size; i++)
	{
		if (strcmp(map->pairs[i].key, key) == 0)
		{
			return map->pairs[i].value;
		}
	}
	printf("KEY: %s NOT FOUND IN MAP\n", key);
	return -1; // Key not found
}

/* HELPER FUNCTION TO HANDLE SETTING UP MOV OPERATIONS */
void helper_MOV(int* i)
{
	char* val = strtok(NULL, " ");             // Get first argument of instruction
	char* val2 = strtok(NULL, " ");            // Get second argument of instruction

	if (isdigit(*val))                         // First value is a digit
	{
		MOV_INFO.results[MOV_INFO.size] = 1;   // TODO: Temporary fix for keeping track of calls with arg states
		MOV_INFO.size++;
		program[*i+1] = atoi(val);             // Add the explicit value to the program array
		program[*i+2] = *val2;                 // The destination register has to be given if int value is explicit
		*i+=2;                                 // Skip the args
		//*i+=2;
	}
	else
	{
		MOV_INFO.results[MOV_INFO.size] = 0;
		MOV_INFO.size++;
		program[*i+1] = *val;
		*i+=1;
	}
}

/* HELPER FUNCTION TO HANDLE SETTING UP POR OPERATIONS */
void helper_POR(int* i)
{
	char* reg = strtok(NULL, " ");
	if (reg)
	{
		program[*i+1] = *reg; // Add register to program array
		*i+=1;
	}
}

/* HELPER FUNCTION TO HANDLE SETTING UP CMP OPERATIONS */
void helper_CMP(int* i)
{
	char* reg = strtok(NULL, " ");
	char* arg2 = strtok(NULL, " ");
	if (reg)
	{
		program[*i+1] = *reg; // First arg should *always* be a register
		if (arg2)
		{
			CMP_INFO.results[CMP_INFO.size] = 1; // TODO: Temporary way to keep track of the args with CMP calls
			CMP_INFO.size++;
			if (isdigit(*arg2)) // 2nd arg can be given as explicit int or another register
			{
				program[*i+2] = atoi(arg2);
			}
			else
			{
				program[*i+2] = *arg2;
			}
			*i+=1;
		}
		else
		{
			CMP_INFO.results[CMP_INFO.size] = 0;
			CMP_INFO.size++;
		}
	}
	else
	{
		fprintf(stderr, "ERROR: Inavlid usage of CMP instruction: %d - CMP {register} {register} or CMP {register} {value}", pc);
	} 
	*i+=1; // Skip args
}

void helper_JMP(int* i)
{
	char* destination = strtok(NULL, " ");            // Get the label text
	destination[strcspn(destination, "\r\n")] = '\0'; // Remove line ending strings
	int count = 0;
	while (labels[count] != NULL)
	{
		count++;
	}
	labels[count] = strdup(destination);              // Put the destination string into the labels array at the latest element
	program[*i+1] = count;                            // Add the index as the JMP argument to the program array
	*i+=1;
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
		int sum = val1 + val2; 			   // Adds the top two values on the stack together
		sp-=2;                              // decrement stack pointer
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

		if (MOV_INFO.index > MOV_INFO.size)
		{
			fprintf(stderr, "ERROR: MOV index out of bounds of MOV size: %d > %d :: pc->%d\n", MOV_INFO.index, MOV_INFO.size, pc);
			running = false;
		}

		// A value is given explicitly
		if (MOV_INFO.results[MOV_INFO.index] == 1)
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
		MOV_INFO.index++;

		dstPtr = getRegister((char)dstReg);
		*dstPtr = value; // Set the register to hold the given value;
						 
		printf("MOV: Register %c now contains value %d\n", dstReg, value);
	}
}

void handle_CMP()
{
	if (sp >= 0)
	{
		int  arg1 = program[++pc];
		int  val1, val2;
		int* pReg;

		// val1 is always going to be a register so just grab the value in that register ahead of time
		pReg = getRegister((char)arg1);
		val1 = *pReg;

		if (CMP_INFO.index > CMP_INFO.size)
		{
			fprintf(stderr, "ERROR: CMP index out of bounds of CMP size: %d > %d :: pc->%d\n", CMP_INFO.index, CMP_INFO.size, pc);
			running = false;
		}
		if (CMP_INFO.results[CMP_INFO.index] == 1)
		{
			int arg2 = program[++pc];
			if (isdigit(arg2))                      // Explicit int given
			{
				val2 = arg2;                        // Just an int
			}
			else                                    // Register given as 2nd arg
			{
				pReg = getRegister((char)arg2);
				val2 = *pReg;
			}
		}
		else // One value is given - compare with top of the stack
		{
			val2 = stack[sp];
		}
		CMP_INFO.index++;

		if (val1 == val2)
		{
			printf("CMP: %d and %d are equal.\n", val1, val2);
			setFlag(&registers.flags, CMP_FLAG); // Tell the evaluator the result
		}
		else
		{
			printf("CMP: %d and %d are NOT equal.\n", val1, val2);
			clearFlag(&registers.flags, CMP_FLAG);
			if (val1 > val2)
			{
				setFlag(&registers.flags, CMP_GT_FLAG); // Set the Greater Than flag
			}
			else
			{
				clearFlag(&registers.flags, CMP_GT_FLAG); // Clear Greater Than flag
			}
		}
	}
	else
	{
		fprintf(stderr, "ERROR: Stack underflow during CMP operation: %d", pc);
	}
}

/* Jump execution to given label */
void handle_JMP()
{
	int index = program[++pc];                    // the index of the destination in the labels array
	char* destination = labels[index];            // Get the destination of the jump, which is the string held in the labels array
	int value = getValue(&LabelMap, destination); // Get the value from the LabelMap - label being the PC destination
	pc = value - 1;
}

/* JMP IF EQUAL */
void handle_JIE()
{
	if (isFlagSet(registers.flags, CMP_FLAG)) // Last CMP result was equal
	{
		handle_JMP();
	}
	else
	{
		pc++;
	}
}

/* JMP NOT EQUAL */
void handle_JNE()
{
	if (!isFlagSet(registers.flags, CMP_FLAG)) // Last CMP result was not equal
	{
		handle_JMP();
	}
	else
	{
		pc++;
	}
}

/* JMP GREATER THAN */
void handle_JGT()
{
	if (isFlagSet(registers.flags, CMP_GT_FLAG)) // Last CMP result was greater than
	{
		handle_JMP();
	}
	else
	{
		pc++;
	}
}

void handle_JLT()
{
	if (!isFlagSet(registers.flags, CMP_GT_FLAG)) // Last CMP result was less than
	{
		handle_JMP();
	}
	else
	{
		pc++;
	}
}

void handle_POR()
{
	if (sp >= 0)
	{
		int reg;
		int* pReg;
		reg = program[++pc];     	    // Get the register from the program array
		pReg = getRegister((char)reg);  // Get a pointer to the register
		stack[++sp] = *pReg;     		// Push the value from the register onto the stack
		printf("POR: Popped value %d from register %c\n", *pReg, reg);
		*pReg = 0;               		// Clear the register
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
		printf("ST: Stored value %d at 0x%X\n", value, registers.F);
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
		printf("LD: Loaded %d from 0x%X\n", value, address);
	}
	else
	{
		fprintf(stderr, "ERROR: Stack underflow during LD operation: %d\n", pc);
		printf("Entered else block in LD\n");
	}
}

bool is_running()
{
	return running;
}

void set_running(bool status)
{
	running = status;
}


