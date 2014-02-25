#!/bin/bash

LPATH=$GOLD_LLVM_BIN

export CC=$LPATH"/clang -use-gold-plugin"
export RANLIB="/bin/true"
export LDFLAGS="-lmonitor -L"$INSTRUMENTOR_PATH"/src -L"$GLOG_PATH"/lib -L"$PROFILER_PATH"/lib" 

# variables related to profiling
#export PROFILER=""
export PROFILER="-lprofiler"
export CPUPROFILE="a.prof"

# compiling code
$CC -c -fpack-struct -emit-llvm -g -pg $1.c -o $1.bc $PROFILER
llvm-dis $1.bc

# instrumenting bitcode file
$LPATH/opt -load $INSTRUMENTOR_PATH/MonitorPass/MonitorPass.so --instrument -f --file $GLOG_log_dir/$1-metadata.txt --includedFunctions $1-include.txt -o tmppass.bc $1.bc

# removing alloca instructions to top of functions
$LPATH/opt -load $INSTRUMENTOR_PATH/MonitorPass/MonitorPass.so --move-allocas -f -o tmppass-allocas.bc tmppass.bc
llvm-dis tmppass-allocas.bc -o $1-inst.ll

# creating executable
$CC tmppass-allocas.bc -o $1.out -L$LDFLAGS -lmonitor -lpthread -lm -lrt -lglog $PROFILER

# to inspect profiling information
# pprof --text file.out a.prof
