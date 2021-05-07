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
			${OBJ_DIR}/test_decinstruction.o \
			${OBJ_DIR}/test_eorinstruction.o \
			${OBJ_DIR}/test_fmulinstruction.o \
			${OBJ_DIR}/test_icallinstruction.o \
			${OBJ_DIR}/test_ijmpinstruction.o \
			${OBJ_DIR}/test_ininstruction.o \
			${OBJ_DIR}/test_incinstruction.o \
			${OBJ_DIR}/test_jmpinstruction.o \
			${OBJ_DIR}/test_lacinstruction.o \
			${OBJ_DIR}/test_lasinstruction.o \
			${OBJ_DIR}/test_latinstruction.o \
			${OBJ_DIR}/test_ldinstruction.o \
			${OBJ_DIR}/test_lddinstruction.o \
			${OBJ_DIR}/test_lddzinstruction.o \
			${OBJ_DIR}/test_ldiinstruction.o \
			${OBJ_DIR}/test_ldsinstruction.o \
			${OBJ_DIR}/test_lpminstruction.o \
			${OBJ_DIR}/test_lsrinstruction.o \
			${OBJ_DIR}/test_movinstruction.o \
			${OBJ_DIR}/test_movwinstruction.o \
			${OBJ_DIR}/test_mulinstruction.o \
			${OBJ_DIR}/test_mulsinstruction.o \
			${OBJ_DIR}/test_mulsuinstruction.o \
			${OBJ_DIR}/test_neginstruction.o \
			${OBJ_DIR}/test_nopinstruction.o

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
