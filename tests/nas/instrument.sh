#!/bin/bash

export CC=$LLVM_BIN_PATH"/clang -use-gold-plugin"
export LDFLAGS="$INSTRUMENTOR_LIB_PATH -L"$GLOG_LIB_PATH" -L"$GPERFTOOLS_LIB_PATH" -L"$UNWIND_LIB_PATH

# variables related to profiling
#export PROFILER=""
export PROFILER="-lprofiler"
export MALLOC="-ltcmalloc"

$LLVM_BIN_PATH/llvm-dis $1.bc -o $1-orig.ll

# remove constant geps
$LLVM_BIN_PATH/opt -load $MONITOR_LIB_PATH/MonitorPass.$SHARED_LIB_EXTENSION --break-constgeps -f -o $1-ngep.bc $1.bc

# instrument the bitcode file
$LLVM_BIN_PATH/opt -load $MONITOR_LIB_PATH/MonitorPass.$SHARED_LIB_EXTENSION --instrument --file $GLOG_log_dir/$1-metadata.txt -f -o tmppass.bc $1-ngep.bc

# move alloca instructions to top of each function
$LLVM_BIN_PATH/opt -load $MONITOR_LIB_PATH/MonitorPass.$SHARED_LIB_EXTENSION --move-allocas -f -o tmppass-allocas.bc tmppass.bc
$LLVM_BIN_PATH/llvm-dis tmppass-allocas.bc -o $1.ll

# create executable
$CC tmppass-allocas.bc -o $1.out -L$LDFLAGS -lmonitor -lpthread -lm -lrt -lgmp -lglog $PROFILER $MALLOC
#ld tmppass-allocas.bc -o $1.out -L$LDFLAGS -lmonitor -lpthread -lm -lrt -lgmp -lglog $PROFILER $MALLOC

# create executable for uninstrumented bitcode
$CC $1.bc -o $1.out2 -L$LDFLAGS -lmonitor -lpthread -lm -lrt -lgmp -lglog $PROFILER $MALLOC

# inspecting profile information
# pprof --text executable.out cpu.prof
# pprof --text executable.out heap.prof

