#!/bin/bash

LPATH=$GOLD_LLVM_BIN
export CC=$LPATH"/clang -use-gold-plugin"
export LDFLAGS="-lmonitor -L"$INSTRUMENTOR_PATH"/src -L"$GLOG_PATH"/lib"

# instrument the bitcode file
$LPATH/opt -load $INSTRUMENTOR_PATH/MonitorPass/MonitorPass.so --instrument -f -o tmppass.bc $1.bc

# create executable 
llc tmppass.bc
$CC tmppass.s -o $1.out -L$LDFLAGS -lmonitor -lpthread -lm -lrt -lglog
#$CC tmppass.s -o $1.out -L$LDFLAGS -lmonitor -lpthread -lm -lrt -lgmp -lglog

# create executable for uninstrumented bitcode
#llc $1.bc
#$CC $1.s -o $1.out2 -L$LDFLAGS -lmonitor -lpthread -lm -lrt -lglog
