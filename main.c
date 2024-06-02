#include "vm_definitions.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

//Registers registers = {0};
//bool running = true;

int main(int argc, char *argv[])
{
	init_type_vars();

	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s <program_file>\n", argv[0]);
		return EXIT_FAILURE;
	}
	//registers.flags = 0;
	loadProgram(argv[1]);
	while (is_running())
	{
		evaluate();
	}
	return 0;
}
