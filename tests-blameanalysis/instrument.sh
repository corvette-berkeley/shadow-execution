#!/bin/bash

export CC=$LLVM_BIN_PATH"/clang -use-gold-plugin"
export LDFLAGS="-L"$INSTRUMENTOR_LIB_PATH" -L"$BLAMEANALYSIS_LIB_PATH" -L"$GLOG_LIB_PATH" -L"$GPERFTOOLS_LIB_PATH" -L"$UNWIND_LIB_PATH" -L"$MONITOR_LIB_PATH

name=$(basename $1 .c)

# Compile into bitcode
$LLVM_BIN_PATH/clang -emit-llvm -g -Xclang -dwarf-column-info -pg -c $name.c -o $name.bc

# Memove constant geps
$LLVM_BIN_PATH/opt -load $MONITOR_LIB_PATH/MonitorPass.so --break-constgeps -f -o $name-ngep.bc $name.bc

# Instrument the bitcode
$LLVM_BIN_PATH/opt -load $MONITOR_LIB_PATH/MonitorPass.so --instrument --file $GLOG_log_dir/debug.bin -f -o tmppass.bc $name-ngep.bc

# Move alloca instructions to top of each function
$LLVM_BIN_PATH/opt -load $MONITOR_LIB_PATH/MonitorPass.so --move-allocas -f -o tmppass-allocas.bc tmppass.bc

# Create executable
$CC tmppass-allocas.bc -o $name.out $LDFLAGS -lbba -lmonitor -lpthread -lm -lrt -lgmp -lglog -ltcmalloc

# Clean temporary files
rm $name.bc
#rm $name-ngep.bc
#rm tmppass.bc
#rm tmppass-allocas.bc
