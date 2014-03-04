
COMPILER = cobc
COBCOPTS = -fixed -Wall

EXECUTABLES = fbench fbench_dec

all:	$(EXECUTABLES)

fbench: fbench.cob
	$(COMPILER) $(COBCOPTS) -x fbench.cob

fbench_dec: fbench_dec.cob
	$(COMPILER) $(COBCOPTS) -x fbench_dec.cob

clean:
	rm -f $(EXECUTABLES) *.hi *.o *.c *.c.h *.c.l.h

time:	fbench
	/usr/bin/time -p ./fbench -100000

supertime:
	nice -n -10 make time

