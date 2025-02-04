# Holds a list of object files for project compilation
OBJECTS=

# Holds compiler flags , -I specifying to look in the current directory
INCLUDES= -I./


ALL: ${OBJECTS}

	  # compiles main.c and links it to files in OBJECTS
   	gcc main.c ${INCLUDES} ${OBJECTS} -g -o ./main
CLEAN:
