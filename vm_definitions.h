void handle_PSH();
void handle_POP();
void handle_ADD();
void handle_SUB();
void handle_MOV();
void handle_ALC();
void handle_FRE();
void handle_ST();
void handle_LD();
void handle_MSG();
void handle_HLT();

void evaluate();
void loadProgram(const char* filename);

typedef enum {
	PSH,
	POP,
	ADD,
	SUB,
	MOV,
	ALC,
	FRE,
	ST,
	LD,
	MSG,
	HLT,
} InstructionSet;

InstructionSet mapStringToEnum(const char* str);

typedef struct {
	int A, B, C, D, E; // General purpose registers
	int PC;
	int SP;            // Program counter and stack pointer
	int* F;            // F register is used to store a pointer / memory address
} Registers;
