#!/bin/bash

LPATH=$GOLD_LLVM_BIN
export CC=$LPATH"/clang -use-gold-plugin"
export LDFLAGS="-lmonitor -L"$INSTRUMENTOR_PATH"/src -L"$GLOG_PATH"/lib -L"$PROFILER_PATH"/lib"

# variables related to profiling
export PROFILER=""
#export PROFILER="-lprofiler"
#export PROFILER="-ltcmalloc"

# remove constant geps
$LPATH/opt -load $INSTRUMENTOR_PATH/MonitorPass/MonitorPass.so --break-constgeps -f -o $1-ngep.bc $1.bc

# instrument the bitcode file
$LPATH/opt -load $INSTRUMENTOR_PATH/MonitorPass/MonitorPass.so --instrument --file $GLOG_log_dir/$1-metadata.txt -f -o tmppass.bc $1-ngep.bc

# move alloca instructions to top of each function
$LPATH/opt -load $INSTRUMENTOR_PATH/MonitorPass/MonitorPass.so --move-allocas -f -o tmppass-allocas.bc tmppass.bc
llvm-dis tmppass-allocas.bc -o $1-inst.ll

# create executable 
$CC tmppass-allocas.bc -o $1.out -L$LDFLAGS -lmonitor -lpthread -lm -lrt -lgmp -lglog $PROFILER
#ld tmppass-allocas.bc -o $1.out -L$LDFLAGS -lmonitor -lpthread -lm -lrt -lgmp -lglog $PROFILER

# create executable for uninstrumented bitcode
$CC $1.bc -o $1.out2 -L$LDFLAGS -lpthread -lm -lrt -lgmp -lglog $PROFILER

# inspecting profile information
# pprof --text executable.out cpu.prof
# pprof --text executable.out heap.prof

