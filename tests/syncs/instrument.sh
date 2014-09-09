#!/bin/bash

export CC=$LLVM_BIN_PATH"/clang -use-gold-plugin"
export RANLIB="/bin/true"
export LDFLAGS="-lmonitor -L"$INSTRUMENTOR_LIB_PATH" -L"$GLOG_LIB_PATH""

$CC -c -fpack-struct -emit-llvm -g $1.c -o $1.bc

$LLVM_BIN_PATH/llvm-dis $1.bc

$LLVM_BIN_PATH/opt -load $MONITOR_LIB_PATH/MonitorPass.$SHARED_LIB_EXTENSION --instrument -f --file $GLOG_log_dir/$1-metadata.txt --includedFunctions $1-include.txt --logfile $1 -o tmppass.bc $1.bc

$LLVM_BIN_PATH/llvm-dis tmppass.bc
$CC tmppass.bc -o $1.out -L$LDFLAGS -lmonitor -lpthread -lm -lrt -lglog

