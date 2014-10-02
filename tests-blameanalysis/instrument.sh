#!/bin/bash

if [ -z "$GOLD_LLVM_BIN"]
then
  LPATH="$(dirname `which clang++`)"
else
  LPATH=$GOLD_LLVM_BIN
fi
export CC=$LPATH"/clang -use-gold-plugin"
export LDFLAGS="-L"$MONITOR_LIB_PATH"/../Release+Asserts/lib -L"$GLOG_LIB_PATH" -L"$PROFILER_PATH"/lib -L"$INSTRUMENTOR_LIB_PATH
echo $LDFLAGS

name=$(basename $1 .c)

# Compile into bitcode
$LPATH/clang -emit-llvm -g -Xclang -dwarf-column-info -pg -c $name.c -o $name.bc

# Memove constant geps
$LPATH/opt -load $MONITOR_LIB_PATH/MonitorPass.so --break-constgeps -f -o $name-ngep.bc $name.bc 

# Instrument the bitcode
$LPATH/opt -load $MONITOR_LIB_PATH/MonitorPass.so --instrument --file $GLOG_log_dir/debug.bin -f -o tmppass.bc $name-ngep.bc

# Move alloca instructions to top of each function
$LPATH/opt -load $MONITOR_LIB_PATH/MonitorPass.so --move-allocas -f -o tmppass-allocas.bc tmppass.bc

# Create executable 
$CC tmppass-allocas.bc -o $name.out $LDFLAGS -lbba -lmonitor -lpthread -lm -lrt -lgmp -lglog # -ltcmalloc

# Clean temporary files
rm $name.bc
#rm $name-ngep.bc
#rm tmppass.bc
#rm tmppass-allocas.bc
