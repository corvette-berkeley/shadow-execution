#!/bin/bash

LPATH="/home/eecs/rubio/build-llvm-3.0/Release/bin/"

export CC=$LPATH"clang -use-gold-plugin"
export RANLIB="/bin/true"
export LDFLAGS="-lmonitor -L/home/eecs/rubio/corvette-instrumentor/src"
export LD_LIBRARY_PATH=/home/eecs/rubio/corvette-instrumentor/src:$LD_LIBRARY_PATH # may need to be done by hand

#$CC -c -emit-llvm -g $1.c -o $1.bc

$LPATH/opt -load ../MonitorPass/MonitorPass.so --instrument -f -o tmppass.bc $1.bc

llc tmppass.bc

$CC tmppass.s -o $1.out -L/home/eecs/rubio/corvette-instrumentor/src -lmonitor -lpthread -lm -lrt
