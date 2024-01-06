SRC = main.c
HDRS = *.h
CC_FLAGS = -Wall -Wextra -g -std=c11
CC = gcc

all:
	${CC} ${SRC} ${HDRS} ${CC_FLAGS} -o svm
