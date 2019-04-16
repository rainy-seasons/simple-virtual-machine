SRC = main.c
CC_FLAGS = -Wall -Wextra -g -std=c11
CC = gcc

all:
	${CC} ${SRC} ${CC_FLAGS} -o svm
