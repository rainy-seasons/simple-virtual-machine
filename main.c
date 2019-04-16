#include <stdio.h>
#include <stdbool.h>

bool running = true;
int stack[256];

typedef enum {
	PSH,
	ADD,
	POP,
	SET,
	HLT
} InstructionSet;

typedef enum{
	A, B, C, D, E, F, // General purpose registers
   	PC,               // program counter / instruction pointer
	SP,               // stack pointer
	REGISTER_SIZE     // easy way to get the size of the registers
} Registers;

#define pc (registers[PC])
#define sp (registers[SP])
static int registers[REGISTER_SIZE];

const int program[] = {
	PSH, 5,
	PSH, 6,
	ADD,
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
		case HLT: {
			running = false;
			break;
		}
		case PSH: {
			sp++;
			stack[sp] = program[++pc]; // push the value following the instruction onto the stack
			break;
		}
		case POP: {
			int val = stack[sp--];      // store the top stack value and decrement stack ptr to pop it
			printf("popped %d\n", val); // for debug purposes
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
