# Holds a list of object files for project compilation
OBJECTS= ./build/compiler.o ./build/cprocess.o ./build/vector.o

# Holds compiler flags , -I specifying to look in the current directory
INCLUDES= -I./


all: ${OBJECTS} # Compiles main.c and links files in OBJECTS list
	gcc main.c ${INCLUDES} ${OBJECTS} -g -o ./main

./build/compiler.o: ./compiler.c # Compiles compiler.c
	gcc ./compiler.c ${INCLUDES} -o ./build/compiler.o -g -c


./build/cprocess.o: ./cprocess.c # Compiles cprocess.c
	gcc ./cprocess.c ${INCLUDES} -o ./build/cprocess.o -g -c

./build/vector.o: ./helpers/vector.c # Compiles vector.c
	gcc ./helpers/vector.c ${INCLUDES} -o ./build/vector.o -g -c


clean:
	rm ./main
	rm -rf ${OBJECTS}
