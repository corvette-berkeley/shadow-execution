#!/bin/bash

LPATH=$GOLD_LLVM_BIN
export CC=$LPATH"/clang -use-gold-plugin"
export LDFLAGS=$INSTRUMENTOR_PATH"/src -L"$GLOG_PATH"/lib -L"$PROFILER_PATH"/lib -L/scratch/rubio/libunwind-1.1.install/lib"

# variables related to profiling
#export PROFILER=""
export PROFILER="-lprofiler"
export MALLOC="-ltcmalloc"

llvm-dis $1.bc -o $1-orig.ll

# remove constant geps
$LPATH/opt -load $INSTRUMENTOR_PATH/MonitorPass/MonitorPass.so --break-constgeps -f -o $1-ngep.bc $1.bc

# instrument the bitcode file
$LPATH/opt -load $INSTRUMENTOR_PATH/MonitorPass/MonitorPass.so --instrument --file $GLOG_log_dir/$1-metadata.txt -f -o tmppass.bc $1-ngep.bc

# move alloca instructions to top of each function
$LPATH/opt -load $INSTRUMENTOR_PATH/MonitorPass/MonitorPass.so --move-allocas -f -o tmppass-allocas.bc tmppass.bc
llvm-dis tmppass-allocas.bc -o $1.ll

# create executable 
$CC tmppass-allocas.bc -o $1.out -L$LDFLAGS -lmonitor -lpthread -lm -lrt -lgmp -lglog $PROFILER $MALLOC
#ld tmppass-allocas.bc -o $1.out -L$LDFLAGS -lmonitor -lpthread -lm -lrt -lgmp -lglog $PROFILER $MALLOC

# create executable for uninstrumented bitcode
$CC $1.bc -o $1.out2 -L$LDFLAGS -lmonitor -lpthread -lm -lrt -lgmp -lglog $PROFILER $MALLOC

# inspecting profile information
# pprof --text executable.out cpu.prof
# pprof --text executable.out heap.prof

