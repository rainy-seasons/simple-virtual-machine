void handle_PSH();
void handle_POP();
void handle_ADD();
void handle_SUB();
void handle_ALLOC();
void handle_FREE();
void handle_ST();
void handle_LD();
void handle_PRNT(); // TODO: this is temporary for debugging
void handle_HLT();

void evaluate();

typedef enum {
	PSH,
	POP,
	ADD,
	SUB,
	ALLOC,
	FREE,
	ST,
	LD,
	PRNT, // TODO: temp instruction for debugging values - just prints the top of the stack
	HLT,
} InstructionSet;

typedef enum{
	A, B, C, D, E,    // General purpose registers
	F,				  // F register is used for storing a memory address
   	PC,               // program counter / instruction pointer
	SP,               // stack pointer
	REGISTER_SIZE     // easy way to get the size of the registers
} Registers;

/* TODO: implement file handling
 * 	this is temporary. Programs will be read from files.*/
const int program[] = {
	PSH, 5,
	PSH, 6,
	ADD,
	PRNT,
	ALLOC, 2,
	ST,
	PRNT,
	PSH, 10,
	LD,
	SUB,
	FREE,
	HLT
};
