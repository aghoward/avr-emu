CC=g++
CXX_FLAGS += --std=c++17 -I./ -Wall -Wextra -Wshadow -Wnon-virtual-dtor -Wold-style-cast -Wcast-align -Wunused -Woverloaded-virtual -pedantic -Wconversion -Wsign-conversion -Wmisleading-indentation

OBJ_DIR =./obj
OBJECTS =${OBJ_DIR}/memory.o ${OBJ_DIR}/executor.o
LIB_DIR =./lib
LIBRARIES =./lib/libinstructions.a

all: ${LIB_DIR} ${OBJ_DIR} avr-emu

avr-emu: main.cc ${OBJECTS} ${LIBRARIES}
	$(CC) ${CXX_FLAGS} -o $@ $< ${OBJECTS} ${LIBRARIES}

${OBJ_DIR}:
	mkdir -p $@

${LIB_DIR}:
	mkdir -p $@

${OBJ_DIR}/%.o: %.cc %.h
	$(CC) ${CXX_FLAGS} -c -o $@ $<

${LIB_DIR}/lib%.a: ./%
	make -C $<

tests: ./tests/*.cc ${LIB_DIR} ${LIBRARIES}
	make -C tests

clean:
	make -C instructions clean
	make -C tests clean
	rm -Rf ${OBJ_DIR} ${LIB_DIR} avr-emu
