#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define STACK_SIZE 256
#define MEMORY_SIZE 1024

bool running = true;
int stack[STACK_SIZE];
int memory[MEMORY_SIZE];

typedef enum {
	PSH,
	ADD,
	POP,
	SET,
	HLT,
	ALLOC,
	FREE,
} InstructionSet;

typedef enum{
	A, B, C, D, E,    // General purpose registers
	F,				  // F register is used for storing a memory address
   	PC,               // program counter / instruction pointer
	SP,               // stack pointer
	REGISTER_SIZE     // easy way to get the size of the registers
} Registers;

#define pc (registers[PC])
#define sp (registers[SP])
static int registers[REGISTER_SIZE];

/* TODO: implement file handling
 * 	this is temporary. Programs will be read from files.*/
const int program[] = {
	PSH, 5,
	PSH, 6,
	ADD,
	ALLOC, 10,
	FREE,
	POP,
	HLT
};

int fetch()
{
	return program[pc];
}

void evaluate(int op)
{
	switch (op)
	{
		case HLT: { // HALT
			running = false;
			break;
		}
		case PSH: {
			sp++;
			stack[sp] = program[++pc]; // push the value following the instruction onto the stack
			break;
		}
		case POP: {
			registers[C] = stack[sp--];      // store the top stack value and decrement stack ptr to pop it
			break;
		}
		case ADD: {
			registers[A] = stack[sp--];            // pop the values off the stack, and store in registers A and B
			registers[B] = stack[sp--];
			int sum = registers[A] + registers[B];
			sp++;                                  // allocate space on the stack
			stack[sp] = sum;                       // push sum onto stack
			break;
		}
		case ALLOC: { // Dynamically allocates memory - Takes one argument specifying size of memory to allocate
			int size = program[++pc];
			int* ptr = malloc(size * sizeof(int));
			if (ptr == NULL)
			{
				fprintf(stderr, "ERROR: Failed to allocate memory.\n");
				running = false;
			}
			else
			{
				sp++;
				registers[F] = (int)ptr; // Pushes the address of allocated memory in the F register
			}
			break;
		}
		case FREE: {                       // Deallocates allocated memory
			int* ptr = (int*)registers[F]; // Gets the address of the allocated memory from the F register
			free(ptr);
			break;
		}
	}
}

int main() 
{
	while (running)
	{
		evaluate(fetch());
		pc++;
	}
}
