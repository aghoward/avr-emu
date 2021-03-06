CC=g++
CXX_FLAGS += --std=c++17 -I./ -I./cdif -Wall -Wextra -Wshadow -Wnon-virtual-dtor -Wold-style-cast -Wcast-align -Wunused -Woverloaded-virtual -pedantic -Wconversion -Wsign-conversion -Wmisleading-indentation
LD_FLAGS +=-lpthread

OBJ_DIR =./obj
OBJECTS =
LIB_DIR =./lib
LIBRARIES =./lib/libinstructions.a ./lib/libcore.a

all: ${LIB_DIR} ${OBJ_DIR} avr-emu

test.o: test.s
	avr-gcc -mmcu=atmega328p -c -o $@ $<

avr-emu: main.cc ${OBJECTS} ${LIBRARIES}
	$(CC) ${CXX_FLAGS} -o $@ $< ${OBJECTS} ${LIBRARIES} ${LD_FLAGS}

${OBJ_DIR}:
	mkdir -p $@

${LIB_DIR}:
	mkdir -p $@

${OBJ_DIR}/%.o: %.cc %.h
	$(CC) ${CXX_FLAGS} -c -o $@ $<

${LIB_DIR}/lib%.a: ./%
	make ${MAKEFLAGS} -C $<

tests/unittests: ./tests/*.cc ${LIB_DIR} ${LIBRARIES}
	make ${MAKEFLAGS} -C tests

tests: tests/unittests

clean:
	make ${MAKEFLAGS} -C instructions clean
	make ${MAKEFLAGS} -C tests clean
	rm -Rf ${OBJ_DIR} ${LIB_DIR} avr-emu
