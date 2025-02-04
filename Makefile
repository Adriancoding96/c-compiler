# Holds a list of object files for project compilation
OBJECTS=

# Holds compiler flags , -I specifying to look in the current directory
INCLUDES= -I./


all: ${OBJECTS} # Compiles main.c and links files in OBJECTS list
	gcc main.c ${INCLUDES} ${OBJECTS} -g -o ./main
clean:
	rm ./main
	rm -rf ${OBJECTS}
