#!/bin/bash

LPATH=$GOLD_LLVM_BIN

export CC=$LPATH"/clang -use-gold-plugin"
export RANLIB="/bin/true"
export LDFLAGS="-lmonitor -L"$INSTRUMENTOR_PATH"/src -L"$GLOG_PATH"/lib" 

$CC -c -fpack-struct -emit-llvm -g $1.c -o $1.bc

llvm-dis $1.bc

$LPATH/opt -load $INSTRUMENTOR_PATH/MonitorPass/MonitorPass.so --instrument -f --file $1-files.txt -o tmppass.bc $1.bc

llvm-dis tmppass.bc
$CC tmppass.bc -o $1.out -L$LDFLAGS -lmonitor -lpthread -lm -lrt -lglog 

