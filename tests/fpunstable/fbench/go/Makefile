
COMPILER = go
BUILDOPTS = 
ITERATIONS = 3000000
TIME = time
TIMESHELL = bash

EXECUTABLES = fbench

fbench: fbench.go
	$(COMPILER) $(BUILDOPTS) build fbench.go

all:	$(EXECUTABLES)

clean:
	$(COMPILER) clean

time:	fbench
	$(TIMESHELL) -c "$(TIME) ./fbench -$(ITERATIONS)"

supertime:	fbench
	nice -n -10 make time

