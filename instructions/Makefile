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
		 ${OBJ_DIR}/eor.o \
		 ${OBJ_DIR}/fmul.o \
		 ${OBJ_DIR}/icall.o \
		 ${OBJ_DIR}/ijmp.o \
		 ${OBJ_DIR}/in.o \
		 ${OBJ_DIR}/inc.o \
		 ${OBJ_DIR}/jmp.o \
		 ${OBJ_DIR}/lac.o \
		 ${OBJ_DIR}/las.o \
		 ${OBJ_DIR}/lat.o \
		 ${OBJ_DIR}/ld.o \
		 ${OBJ_DIR}/ldd.o \
		 ${OBJ_DIR}/lddz.o \
		 ${OBJ_DIR}/ldi.o \
		 ${OBJ_DIR}/lds.o \
		 ${OBJ_DIR}/lpm.o \
		 ${OBJ_DIR}/lsr.o \
		 ${OBJ_DIR}/mov.o \
		 ${OBJ_DIR}/movw.o \
		 ${OBJ_DIR}/mul.o \
		 ${OBJ_DIR}/muls.o \
		 ${OBJ_DIR}/mulsu.o \
		 ${OBJ_DIR}/neg.o \
		 ${OBJ_DIR}/nop.o \
		 ${OBJ_DIR}/or.o \
		 ${OBJ_DIR}/ori.o \
		 ${OBJ_DIR}/out.o \
		 ${OBJ_DIR}/pop.o \
		 ${OBJ_DIR}/push.o \
		 ${OBJ_DIR}/rcall.o \
		 ${OBJ_DIR}/ret.o \
		 ${OBJ_DIR}/reti.o \
		 ${OBJ_DIR}/rjmp.o \
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
