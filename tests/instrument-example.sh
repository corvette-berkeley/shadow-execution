#!/bin/bash

#LPATH="/home/eecs/nacuong/projects/corvette-instrumentor/lib/llvm-3.0.src/build-llvm/Release/bin/"
LPATH=$GOLD_LLVM_BIN

export CC=$LPATH"/clang -use-gold-plugin"
export RANLIB="/bin/true"
export LDFLAGS="-lmonitor -L"$INSTRUMENTOR_PATH"/src"
#export LD_LIBRARY_PATH=/home/eecs/nacuong/projects/corvette-instrumentor/src:$LD_LIBRARY_PATH # may need to be done by hand

$CC -c -emit-llvm -g $1.c -o $1.bc

$LPATH/opt -load ../MonitorPass/MonitorPass.so --instrument -f -o tmppass.bc $1.bc

llc tmppass.bc

$CC tmppass.s -o $1.out -L$LDFLAGS -lmonitor -lpthread -lm -lrt
