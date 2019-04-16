typedef enum {
	PSH,
	ADD,
	POP,
	SET,
	HLT
} InstructionSet;

const int testProgram[] = {
	PSH, 5,
	PSH, 6,
	ADD,
	POP,
	HLT
};

