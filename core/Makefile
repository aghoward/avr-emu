CC=g++
AR=ar
CXX_FLAGS += --std=c++17 -I../ -I../cdif -Wall -Wextra -Wshadow -Wnon-virtual-dtor -Wold-style-cast -Wcast-align -Wunused -Woverloaded-virtual -pedantic -Wconversion -Wsign-conversion -Wmisleading-indentation
AR_FLAGS +=rcs

OBJ_DIR =./obj
OBJECTS =${OBJ_DIR}/memory.o \
		 ${OBJ_DIR}/executor.o \
		 ${OBJ_DIR}/clock.o \
		 ${OBJ_DIR}/noopclock.o

LIB_DIR =../lib
LIBRARIES =${LIB_DIR}/libcore.a

all: ${OBJ_DIR} ${LIB_DIR} ${OBJECTS} ${LIBRARIES}

${OBJ_DIR}:
	mkdir -p $@

${OBJ_DIR}/%.o: %.cc %.h
	$(CC) ${CXX_FLAGS} -c -o $@ $<

${LIB_DIR}/lib%.a: ${OBJECTS}
	$(AR) ${AR_FLAGS} $@ ${OBJECTS}

clean:
	rm -Rf ${OBJ_DIR} ${LIBRARIES}
