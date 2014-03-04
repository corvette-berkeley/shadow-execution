
COMPILER = ghc
GHCOPTS = -O2 -Wall -funbox-strict-fields
#	To enable +RTS options for garbage collection statistics in timing tests
# GHCOPTS = -O2 -Wall -funbox-strict-fields -rtsopts
ITERATIONS = 5551443

EXECUTABLES = fbench fbench_baseline fbench_pure

fbench: fbench.hs
	$(COMPILER) $(GHCOPTS) --make fbench.hs -o fbench

fbench_baseline: fbench_baseline.hs
	$(COMPILER) $(GHCOPTS) --make fbench_baseline.hs -o fbench_baseline

fbench_pure: fbench_pure.hs
	$(COMPILER) $(GHCOPTS) --make fbench_pure.hs -o fbench_pure

all:	$(EXECUTABLES)

clean:
	rm -f $(EXECUTABLES) *.hi *.o

time:	fbench
	/usr/bin/time -p -a -o \
		/tmp/bench_hs.txt \
		./fbench -$(ITERATIONS) +RTS -sstderr 2> /tmp/bench_hs.txt

supertime:
	nice -n -10 make time

