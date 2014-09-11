#!/bin/bash

LPATH=$GOLD_LLVM_BIN
export CC=$LPATH"/clang -use-gold-plugin"
export LDFLAGS="-L"$INSTRUMENTOR_PATH"/Release+Asserts/lib/ -L"$GLOG_PATH"/lib -L"$PROFILER_PATH"/lib"

name=$(basename $1 .c)

# Compile into bitcode
$LPATH/clang -emit-llvm -g -pg -c $name.c -o $name.bc

# Memove constant geps
$LPATH/opt -load $INSTRUMENTOR_PATH/MonitorPass/MonitorPass.so --break-constgeps -f -o $name-ngep.bc $name.bc 

# Instrument the bitcode
$LPATH/opt -load $INSTRUMENTOR_PATH/MonitorPass/MonitorPass.so --instrument --file $GLOG_log_dir/$name-metadata.txt -f -o tmppass.bc $name-ngep.bc

# Move alloca instructions to top of each function
$LPATH/opt -load $INSTRUMENTOR_PATH/MonitorPass/MonitorPass.so --move-allocas -f -o tmppass-allocas.bc tmppass.bc

# Create executable 
$CC tmppass-allocas.bc -o $name.out $LDFLAGS -lba -lpthread -lm -lrt -lgmp -lglog

# Clean temporary files
rm $name.bc
rm $name-ngep.bc
rm tmppass.bc
rm tmppass-allocas.bc
