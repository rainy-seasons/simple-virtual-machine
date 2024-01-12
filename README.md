# SVM
A simple virtual machine used to run programs in an imaginary instruction set. Still in very early development, working on basic functionality and instructions.

## Instructions
```
	PSH - Pushes a value onto the stack
	POP - Pops a value from the stack
	POR - Pops a value from a given register back onto the stack
	ADD - Adds the two values on top of the stack together
	SUB - Subtracts top two values of the stack
	MOV - Moves a value into a given register. Takes value from top of the stack if no value is given.
	ALC - Allocates a specified number of bytes of memory and stores the pointer in the F register
	FRE - Frees the allocated memory
	ST  - Stores a value into allocated memory
	LD  - Loads a value from allocated memory
	MSG - Just prints the value on top of the stack for now
	HLT - Halts execution of the program
```

## Registers
```
A, B, C, D, E - General purpose registers. Can be accessed at will.
F,            - Pointer register. Stores a memory address to dynamically allocated memory.
PC, SP,       - Program counter and stack pointer. Can not be accessed.
flags,        - Holds flag information in a bit field
```

## Example
```
; This program just moves values around in order to illustrate usage
; Note: Comments are not yet implemented.

PSH 5     ; Pushing values onto the stack
PSH 6
ADD       ; Adding top values together to get 11
MOV A     ; Pops 11 off the stack into the A register
PSH 20
POR A     ; Pops 11 out of the A register back onto the stack
SUB       ; Subtracts 20-11 = 9
MOV 25 B  ; Moves 25 into the B register
ALC 2     ; Allocate 2 bytes of memory
ST        ; Pops 9 off the stack and stores it into the newly allocated memory.
PSH 5
LD        ; Loads 9 from the allocated memory address
POR B     ; Pop 25 from B register back onto the stack
ADD       ; Add 9+25
HLT       ; Halt execution
```
