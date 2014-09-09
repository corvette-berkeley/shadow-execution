#!/bin/bash


export CC=$LLVM_BIN_PATH"/clang -use-gold-plugin"
export RANLIB="/bin/true"
export LDFLAGS="-lmonitor -L"$INSTRUMENTOR_LIB_PATH""

$CC -c -emit-llvm -g $1.c -o $1.bc

$LLVM_BIN_PATH/llvm-dis $1.bc

$LLVM_BIN_PATH/opt -load ../../MonitorPass/MonitorPass.so --instrument -f -o tmppass.bc $1.bc

$LLVM_BIN_PATH/llvm-dis tmppass.bc
$LLVM_BIN_PATH/llc tmppass.bc

$CC tmppass.s -o $1.out -L$LDFLAGS -lmonitor -lpthread -lm -lrt
