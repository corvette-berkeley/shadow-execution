#!/bin/bash

export CC=$LLVM_BIN_PATH"/clang -use-gold-plugin"
export LDFLAGS="-lmonitor -L"$INSTRUMENTOR_LIB_PATH" -L"$GLOG_LIB_PATH" -L"$GPERFTOOLS_LIB_PATH""

# variables related to profiling
export PROFILER=""
# export PROFILER="-lprofiler"
export CPUPROFILE="a.prof"

#compile into bitcode
$LLVM_BIN_PATH/clang -emit-llvm -g -pg -c $1.c -o $1.bc

# remove constant geps
$LLVM_BIN_PATH/opt -load $MONITOR_LIB_PATH/MonitorPass.$SHARED_LIB_EXTENSION --break-constgeps -f -o $1-ngep.bc $1.bc

# instrument the bitcode file
$LLVM_BIN_PATH/opt -load $MONITOR_LIB_PATH/MonitorPass.$SHARED_LIB_EXTENSION --instrument --file $GLOG_log_dir/$1-metadata.txt -f -o tmppass.bc $1-ngep.bc
$LLVM_BIN_PATH/llvm-dis $1-ngep.bc

# move alloca instructions to top of each function
$LLVM_BIN_PATH/opt -load $MONITOR_LIB_PATH/MonitorPass.$SHARED_LIB_EXTENSION --move-allocas -f -o tmppass-allocas.bc tmppass.bc
$LLVM_BIN_PATH/llvm-dis tmppass-allocas.bc -o $1-inst.ll

# create executable
$CC tmppass-allocas.bc -o $1.out -L$LDFLAGS -lmonitor -lpthread -lm -lrt -lgmp -lglog $PROFILER

# create executable for uninstrumented bitcode
$CC $1.bc -o $1.out2 -L"$GLOG_LIB_PATH" -lpthread -lm -lrt -lglog

# inspect profiling information
# pprof --text file.out a.prof

