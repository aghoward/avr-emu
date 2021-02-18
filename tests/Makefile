GCC ?= g++
CXX_FLAGS += --std=c++17 -I../ -Wall -Wextra -Wshadow -Wnon-virtual-dtor -Wold-style-cast -Wcast-align -Wunused -Woverloaded-virtual -pedantic -Wconversion -Wsign-conversion -Wmisleading-indentation
LD_FLAGS = -lgtest_main -lgtest

LIB_DIR = ../lib
OBJ_DIR = ./obj
LIBRARIES = ${LIB_DIR}/libinstructions.a ${LIB_DIR}/libcore.a
TEST_OBJS = ${OBJ_DIR}/test_addinstruction.o \
			${OBJ_DIR}/test_adiwinstruction.o \
			${OBJ_DIR}/test_andinstruction.o \
			${OBJ_DIR}/test_andiinstruction.o \
			${OBJ_DIR}/test_asrinstruction.o \
			${OBJ_DIR}/test_bclrinstruction.o \
			${OBJ_DIR}/test_bldinstruction.o \
			${OBJ_DIR}/test_brbcinstruction.o \
			${OBJ_DIR}/test_bstinstruction.o \
			${OBJ_DIR}/test_callinstruction.o \
			${OBJ_DIR}/test_cbiinstruction.o \
			${OBJ_DIR}/test_cominstruction.o \
			${OBJ_DIR}/test_cpinstruction.o \
			${OBJ_DIR}/test_cpseinstruction.o \
			${OBJ_DIR}/test_decinstruction.o

all: ${OBJ_DIR} unittests

clean:
	rm -Rf unittests ${OBJ_DIR}

${OBJ_DIR}:
	mkdir -p ${OBJ_DIR}

${OBJ_DIR}/%.o: %.cc
	$(GCC) -o $@ -c ${CXX_FLAGS} $<

${LIB_DIR}/%.a:
	make -C ..

unittests: ${TEST_OBJS}
	$(GCC) -o $@ ${CXX_FLAGS} ${TEST_OBJS} ${LIBRARIES} ${LD_FLAGS}
