CC = /usr/bin/g++
nCFLAGS = -O0
dll	= -ldl -rdynamic 
pthread = -lpthread -lrt
prof = -pg
gdb = -ggdb
math = -lm
libz2 = -lbz2
DEBUG_LEVEL = 0

CFLAGS_OBJ = $(nCFLAGS) -DDEBUG_LEVEL=$(DEBUG_LEVEL) $(gdb)
CFLAGS = $(nCFLAGS) -DDEBUG_LEVEL=$(DEBUG_LEVEL) $(math) $(libz2) $(pthread) $(gdb)

PNAME = skynet

OBJ = obj/nn/nn.o obj/utils/utils.o obj/main.o obj/World.o \
obj/Individual.o obj/Util.o obj/Brain.o obj/GeneticsMatrix.o \
obj/NWAlign.o obj/Bzip2.o obj/OS2NN.o obj/NN2OS.o obj/LinuxGrammar.o obj/Genome_IO.o

# default
all: $(OBJ)
	###########################################################
	# This is the Makefile for project "eco"
	# - For nice output formatting, use DEBUG_LEVEL = 2.
	# - The higher the more detailed (and messy) the output is.
	############################################################
		$(CC) $(CFLAGS) -o $(PNAME) $(OBJ) $(LDLFLAGS)

# targets to compile the source files to objectfiles
obj/%.o: src/%.c	
	gcc $(CFLAGS_OBJ) -c $< -o $@

obj/%.o: src/%.cpp
	$(CC) $(CFLAGS_OBJ) -c $< -o $@

# clean target
clean:
		rm $(OBJ) $(PNAME)
