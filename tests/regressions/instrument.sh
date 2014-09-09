#!/bin/bash

export CC=$LLVM_BIN_PATH"/clang -use-gold-plugin"
export RANLIB="/bin/true"
export LDFLAGS="-L"$INSTRUMENTOR_LIB_PATH" -L"$GLOG_LIB_PATH" -L"$GPERFTOOLS_LIB_PATH""

# variables related to profiling
#export PROFILER=""
export PROFILER="-lprofiler"

platform=`uname -s`

if [ "$platform" == "Darwin" ]; then
    libmonitor="MonitorPass.dylib"
else
    libmonitor="MonitorPass.so"
fi

# compiling code
$CC -c -fpack-struct -emit-llvm -g $1.c -o $1.bc
$LLVM_BIN_PATH/llvm-dis $1.bc -o $1-orig.ll

# removing constant geps
$LLVM_BIN_PATH/opt -load $MONITOR_LIB_PATH/$libmonitor --break-constgeps -f -o $1-ngep.bc $1.bc

# instrumenting bitcode file
$LLVM_BIN_PATH/opt -load $MONITOR_LIB_PATH/$libmonitor --instrument -f --file $GLOG_log_dir/$1-metadata.txt --includedFunctions $1-include.txt -o tmppass.bc $1-ngep.bc

# removing alloca instructions to top of functions
$LLVM_BIN_PATH/opt -load $MONITOR_LIB_PATH/$libmonitor --move-allocas -f -o tmppass-allocas.bc tmppass.bc
$LLVM_BIN_PATH/llvm-dis tmppass-allocas.bc -o $1.ll

# creating executable
$CC tmppass-allocas.bc -o $1.out $LDFLAGS -lmonitor -lpthread -lm -lglog $PROFILER #foo.o

# creating executable for original program
$CC $1.bc -o $1-orig.out $LDFLAGS -lmonitor -lpthread -lm -lglog $PROFILER #foo.o

# to inspect profiling information
# pprof --text file.out a.prof
