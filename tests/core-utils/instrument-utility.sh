#!/bin/bash

LPATH=$GOLD_LLVM_BIN
export CC=$LPATH"/clang -use-gold-plugin"
export LDFLAGS="-lmonitor -L"$INSTRUMENTOR_PATH"/src"

# instrument the bitcode file
$LPATH/opt -load $INSTRUMENTOR_PATH/MonitorPass/MonitorPass.so --instrument -f -o tmppass.bc $1.bc

# create executable 
llc tmppass.bc
$CC tmppass.s -o $1.out -L$LDFLAGS -lmonitor -lpthread -lm -lrt
#$CC tmppass.s -o $1.out -L$LDFLAGS -lmonitor -lpthread -lm -lrt -L/home/eecs/rubio/build-gmp-5.1.3/lib -lgmp
