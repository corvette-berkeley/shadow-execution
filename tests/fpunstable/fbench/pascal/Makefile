
GPC_OPT = -O3 --no-stack-checking

FPC_OPT = -O3

all:	fbench_7185 fbench_10206 fbench_borland

fbench_7185: fbench_7185.p
	gpc -ofbench_7185_gpc $(GPC_OPT) -Wwarnings --classic-pascal --pedantic fbench_7185.p
	fpc -ofbench_7185_fpc $(FPC_OPT) fbench_7185.p

fbench_10206: fbench_10206.p
	gpc -ofbench_10206_gpc $(GPC_OPT) -Wwarnings --extended-pascal fbench_10206.p
	fpc -ofbench_10206_fpc $(FPC_OPT) fbench_10206.p

fbench_borland: fbench_borland.p
	gpc -ofbench_borland_gpc $(GPC_OPT) -Wwarnings fbench_borland.p
	fpc -ofbench_borland_fpc $(FPC_OPT) fbench_borland.p

clean:
	rm -f fbench_*_fpc fbench_*_gpc *.o
