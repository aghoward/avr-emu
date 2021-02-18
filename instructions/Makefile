CC=g++
AR=ar
CXX_FLAGS += --std=c++17 -I../ -I../cdif -Wall -Wextra -Wshadow -Wnon-virtual-dtor -Wold-style-cast -Wcast-align -Wunused -Woverloaded-virtual -pedantic -Wconversion -Wsign-conversion -Wmisleading-indentation
AR_FLAGS +=rcs

OBJ_DIR =./obj
OBJECTS =${OBJ_DIR}/notimplemented.o \
		 ${OBJ_DIR}/add.o \
		 ${OBJ_DIR}/adiw.o \
		 ${OBJ_DIR}/and.o \
		 ${OBJ_DIR}/andi.o \
		 ${OBJ_DIR}/asr.o \
		 ${OBJ_DIR}/bclr.o \
		 ${OBJ_DIR}/bld.o \
		 ${OBJ_DIR}/brbc.o \
		 ${OBJ_DIR}/break.o \
		 ${OBJ_DIR}/bst.o \
		 ${OBJ_DIR}/call.o \
		 ${OBJ_DIR}/cbi.o \
		 ${OBJ_DIR}/com.o \
		 ${OBJ_DIR}/cp.o \
		 ${OBJ_DIR}/cpi.o \
		 ${OBJ_DIR}/cpse.o \
		 ${OBJ_DIR}/dec.o \
		 ${OBJ_DIR}/instructionmodule.o

LIB_DIR =../lib
LIBRARIES =${LIB_DIR}/libinstructions.a

all: ${OBJ_DIR} ${LIB_DIR} ${OBJECTS} ${LIBRARIES}

${OBJ_DIR}:
	mkdir -p $@

${OBJ_DIR}/%.o: %.cc %.h
	$(CC) ${CXX_FLAGS} -c -o $@ $<

${LIB_DIR}/lib%.a: ${OBJECTS}
	$(AR) ${AR_FLAGS} $@ ${OBJECTS}

clean:
	rm -Rf ${OBJ_DIR} ${LIBRARIES}
