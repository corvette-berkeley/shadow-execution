#!/bin/bash

LPATH=$GOLD_LLVM_BIN

export CC=$LPATH"/clang -use-gold-plugin"
export RANLIB="/bin/true"
export LDFLAGS="-lmonitor -L"$INSTRUMENTOR_PATH"/src -L"$GLOG_PATH"/lib" 
loggingPath=$CORVETTE_PATH"/logging"

$LPATH/opt -load $INSTRUMENTOR_PATH/MonitorPass/MonitorPass.so --break-constgeps -f -o $1-ngep.bc $1.bc

llvm-dis $1-ngep.bc

$LPATH/opt -load $INSTRUMENTOR_PATH/MonitorPass/MonitorPass.so --instrument -f --file $GLOG_log_dir/$1-metadata.txt --includedFunctions $1-include.txt -o tmppass.bc $1-ngep.bc

$LPATH/opt -load $INSTRUMENTOR_PATH/MonitorPass/MonitorPass.so --move-allocas -f -o tmppass-allocas.bc tmppass.bc

gcc -c $loggingPath/cov_serializer.c -o $2/cov_serializer.o
$CC tmppass-allocas.bc -o $1.out -L$LDFLAGS -lmonitor -lpthread -lm -lrt -lglog $2/cov_serializer.o
llvm-dis tmppass-allocas.bc

# create executable for uninstrumented bitcode
$CC $1.bc -o $2/$1.out2 -L$LDFLAGS -lpthread -lm -lrt -lgmp -lglog $2/cov_serializer.o

