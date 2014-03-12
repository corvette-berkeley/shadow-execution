
#   Options for compiling and linking C fbench programs

CC = $(LLVM_WRAPPER)/wllvm

COPTS = -O3 -lm -g

# PROGRAMS = fbench ffbench fbench_ansi
PROGRAMS = ffbench fbench_ansi

all:	$(PROGRAMS)

#fbench: fbench.c
#	$(CC) fbench.c -o fbench $(COPTS)

fbench_ansi: fbench_ansi.c
	$(CC) -Wall fbench_ansi.c -o fbench_ansi $(COPTS)

ffbench: ffbench.c
	$(CC) ffbench.c -o ffbench $(COPTS)

clean:
	rm -f $(PROGRAMS) *.o *.out core*
