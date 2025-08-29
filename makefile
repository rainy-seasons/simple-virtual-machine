SVM_SRC = main.c vm_definitions.c 
SVM_HDRS = vm_definitions.h 

TEST_SRC = test/test_vm.c vm_definitions.c lib/Unity/src/unity.c
TEST_HDRS = lib/Unity/src/unity.h vm_definitions.h

CC_FLAGS = -Wall -Wextra -g -std=c11
CC = gcc

SVM_OUT = svm
TEST_OUT = test_vm

all: $(SVM_OUT)

$(SVM_OUT): $(SVM_SRC) $(SVM_HDRS)
	${CC} $(SVM_SRC) ${CC_FLAGS} -o $(SVM_OUT)

test: $(TEST_OUT)

$(TEST_OUT): $(TEST_SRC) $(TEST_HDRS)
	${CC} $(TEST_SRC) $(CC_FLAGS) -IUnity/include -o $(TEST_OUT)
