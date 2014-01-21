#!/bin/bash

LPATH=$GOLD_LLVM_BIN

export CC=$LPATH"/clang -use-gold-plugin"
export RANLIB="/bin/true"
export LDFLAGS="-lmonitor -L"$INSTRUMENTOR_PATH"/src"

$CC -c -emit-llvm -g $1.c -o $1.bc

llvm-dis $1.bc

$LPATH/opt -load ../../MonitorPass/MonitorPass.so --instrument -f -o tmppass.bc $1.bc

llvm-dis tmppass.bc
llc tmppass.bc

$CC tmppass.s -o $1.out -L$LDFLAGS -lmonitor -lpthread -lm -lrt
