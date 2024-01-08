void handle_PSH();
void handle_POP();
void handle_ADD();
void handle_SUB();
void handle_ALLOC();
void handle_FREE();
void handle_ST();
void handle_LD();
void handle_PRNT();
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
	PRNT,
	HLT,
} InstructionSet;

typedef struct {
	int A, B, C, D, E; // General purpose registers
	int PC;
	int SP;            // Program counter and stack pointer
	int* F;            // F register is used to store a pointer / memory address
} Registers;

/* TODO: implement file handling
 * 	this is temporary. Programs will be read from files.*/
const int program[] = {
	PSH, 5,
	PSH, 6,
	ADD,
	PRNT,
	ALLOC, 2,
	PRNT,
	ST,
	PRNT,
	LD,
	PRNT,
	PSH, 10,
	SUB,
	PRNT,
	FREE,
	HLT
};
