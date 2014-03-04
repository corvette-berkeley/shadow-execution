
WEBDIR = $(HOME)/web/fbench

#   Options for compiling and linking FORTRAN programs
FORTRANOPTS = -O3

all:
	@echo "Please make for a specified language: c fortran ada pascal haskell cobol go"

c:	c/fbench.c c/ffbench.c Makefile c/Makefile
	( cd c; make clean; make )

fortran:	fortran/fbench.f Makefile
	( cd fortran ; gfortran $(FORTRANOPTS) fbench.f -o fbench )
	
ada:	ada/fbench.adb Makefile
	( cd ada ; rm -f fbench.o fbench ; gnatmake $(ADAOPTS) fbench.adb )
	
pascal: pascal/fbench_borland.p Makefile
	( cd pascal; make clean; make )
	
haskell:	haskell/fbench.hs Makefile haskell/Makefile
	( cd haskell; make clean; make )
	
cobol:	cobol/fbench.cob cobol/fbench_dec.cob Makefile
	( cd cobol; make clean; make )
	
go:	go/fbench.go Makefile
	( cd go; make clean; make )

dist:
	make clean
	rm -f fbench.zip
	zip -r fbench.zip Makefile \
	    c \
	    ada/fbench.adb \
	    cobol \
	    forth \
	    fortran/fbench.f \
	    go \
	    haskell \
	    java \
	    javascript \
	    lisp/fbench.lsp \
	    mbasic \
	    pascal \
	    perl \
	    python \
	    ruby \
	    scilab \
	    smalltalk/fbench.st smalltalk/results.txt \
	    visualbasic

release:
	cp -p fbench.zip $(WEBDIR)

clean:
	rm -f $(PROGRAMS) fortran/fbench *.o *.out core *.bak *.zip
	( cd c; make clean )
	( cd cobol; make clean )
	( cd go; make clean )
	( cd pascal; make clean )
	( cd haskell; make clean )

