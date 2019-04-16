#include <stdbool.h>

bool running = true;
int pc = 0; // program counter / instruction pointer

typedef enum {
	PSH,
	ADD,
	POP,
	SET,
	HLT
} InstructionSet;

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
		case HLT:
			running = false;
			break;
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
