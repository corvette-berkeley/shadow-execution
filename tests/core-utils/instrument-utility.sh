#!/bin/bash

export CC=$LLVM_BIN_PATH"/clang -use-gold-plugin"
export LDFLAGS="-lmonitor -L"$INSTRUMENTOR_LIB_PATH" -L"$GLOG_LIB_PATH""

# remove constant geps
$LLVM_BIN_PATH/opt -load $MONITOR_LIB_PATH/MonitorPass.$SHARED_LIB_EXTENSION --break-constgeps -f -o $1-ngep.bc $1.bc

# instrument the bitcode file
$LLVM_BIN_PATH/opt -load $MONITOR_LIB_PATH/MonitorPass.$SHARED_LIB_EXTENSION --instrument --file $GLOG_log_dir/$1-metadata.txt --includedFunctions $1-include.txt --logfile $1 -f -o tmppass.bc $1-ngep.bc
$LLVM_BIN_PATH/llvm-dis $1-ngep.bc

# create executable
$CC tmppass.bc -o $1.out -L$LDFLAGS -lmonitor -lpthread -lm -lrt -lgmp -lglog

llvm-dis tmppass.bc -o $1-instr.ll

# create executable for uninstrumented bitcode
#$CC $1.bc -o $1.out2 -L"$GLOG_LIB_PATH" -lpthread -lm -lrt -lglog
