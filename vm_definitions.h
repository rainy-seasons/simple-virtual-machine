#include <stdio.h>

void handle_PSH();
void handle_POP();
void handle_POR();
void handle_ADD();
void handle_SUB();
void handle_MUL();
void handle_DIV();
void handle_MOV();
void handle_CMP();
void handle_JMP();
void handle_JIE();
void handle_JNE();
void handle_ALC();
void handle_FRE();
void handle_ST();
void handle_LD();
void handle_MSG();
void handle_HLT();

void helper_MOV(int* i);
void helper_POR(int* i);
void helper_CMP(int* i);
void helper_JMP(int* i);

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
	CMP, 	// Does a comparison and flips the CMP_FLAG if values are equal. Can be given two registers, or one value and one register.
	JMP,    // Jumps the instruction pointer to a specified point of execution
	JIE,	// Jumps to specified point of execution if CMP_FLAG is set (operation was equal / was true)
	JNE,    // Jumps to specified point of execution if CMP_FLAG is *not* set (operation was not equal / was false)
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

typedef struct{
	char key[50];
	int  value;
} KeyValuePair;

typedef struct {
	KeyValuePair pairs[100];
	int size;
} KeyValueMap;


void addToMap(KeyValueMap* map, const char* key, int value);
int getValue(KeyValueMap* map, const char* key);
