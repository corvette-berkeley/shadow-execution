#!/bin/bash

LPATH="/home/eecs/nacuong/projects/corvette-instrumentor/lib/llvm-3.0.src/bin/bin/"
# LPATH="/home/eecs/rubio/build-llvm-3.0/Release/bin/"
export CC=$LPATH"clang -use-gold-plugin"

# instrument the bitcode file
$LPATH/opt -load ../MonitorPass/MonitorPass.so --instrument -f -o tmppass.bc $1.bc

# create executable 
#llc tmppass.bc
#$CC tmppass.s -o $1.out -L/home/eecs/nacuong/projects/corvette-instrumentor/src -lmonitor -lpthread -lm -lrt
