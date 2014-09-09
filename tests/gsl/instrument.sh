#!/bin/bash

export CC=$LLVM_BIN_PATH"/clang -use-gold-plugin"
export RANLIB="/bin/true"
export LDFLAGS="-lmonitor -L"$INSTRUMENTOR_LIB_PATH" -L"$GLOG_LIB_PATH""
loggingPath=$CORVETTE_PATH"/logging"

$LLVM_BIN_PATH/opt -load $MONITOR_LIB_PATH/MonitorPass.$SHARED_LIB_EXTENSION --break-constgeps -f -o $1-ngep.bc $1.bc

$LLVM_BIN_PATH/llvm-dis $1-ngep.bc

$LLVM_BIN_PATH/opt -load $MONITOR_LIB_PATH/MonitorPass.$SHARED_LIB_EXTENSION --instrument -f --file $GLOG_log_dir/$1-metadata.txt --includedFunctions $1-include.txt -o tmppass.bc $1-ngep.bc

$LLVM_BIN_PATH/opt -load $MONITOR_LIB_PATH/MonitorPass.$SHARED_LIB_EXTENSION --move-allocas -f -o tmppass-allocas.bc tmppass.bc

gcc -c $loggingPath/cov_serializer.c -o $2/cov_serializer.o
$CC tmppass-allocas.bc -o $1.out -L$LDFLAGS -lmonitor -lpthread -lm -lrt -lglog $2/cov_serializer.o
$LLVM_BIN_PATH/llvm-dis tmppass-allocas.bc

# create executable for uninstrumented bitcode
$CC $1.bc -o $2/$1.out2 -L$LDFLAGS -lpthread -lm -lrt -lgmp -lglog $2/cov_serializer.o

