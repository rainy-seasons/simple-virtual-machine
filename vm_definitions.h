#include <stdio.h>

void handle_PSH();
void handle_POP();
void handle_POR();
void handle_ADD();
void handle_SUB();
void handle_MUL();
void handle_DIV();
void handle_MOV();
void handle_ALC();
void handle_FRE();
void handle_ST();
void handle_LD();
void handle_MSG();
void handle_HLT();

void helper_MOV(FILE* file, int* i);
void helper_POR(FILE* file, int* i);

int* getRegister(char reg);

void evaluate();
void loadProgram(const char* filename);

void setFlag(int* flags, int flag);
void clearFlag(int* flags, int flag);
int isFlagSet(int flags, int flag);

typedef enum {
	PSH, 	// Pushes a value onto the stack
	POP, 	// Pops a value off the stack
	POR,    // Pops a value from a register onto the stack
	ADD, 	// Adds the two values on top of the stack and pushes the result to the top
	SUB, 	// Subtracts the two values on top of the stack and pushes result
	MUL,    // Multiplies two values on top of the stack and pushes the result
	DIV,    // Divides two values on top of the stack and pushes the result
	MOV, 	// Moves a value into a specified register. Uses value on top of the stack if no value is given
	ALC, 	// Allocates a specified number of bytes of memory and stores the pointer in F register - TODO: Store the pointer on the stack to give flexibility
	FRE, 	// Frees allocated memory
	ST,  	// Stores a value into allocated memory - TODO: Give more flexibility with registers and pointers
	LD,  	// Loads a value from allocated memory to the stack - TODO: Give more flexibility with registers and pointers
	MSG, 	// Just prints the value on top of the stack for now
	HLT, 	// Halts the program
	INSTRUCTION_COUNT, // Easy way to retrieve the number of instructions
} InstructionSet;

InstructionSet mapStringToEnum(const char* str);

typedef struct {
	int A, B, C, D, E; // General purpose registers
	int PC;			   // Program counter / Instruction pointer
	int SP;            // Stack pointer
	int* F;            // F register is used to store a pointer / memory address
	int flags;		   // Flags field
} Registers;
