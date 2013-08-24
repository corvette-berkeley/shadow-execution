#!/bin/bash



LPATH="/home/eecs/rubio"

#pushd $LPATH/llvm-instrument
#source setup.sh
#export_binutils
#popd

clang -c -emit-llvm -g $1.c -o $1.bc

opt -load ../MonitorPass/Passes.so --instrument -f -o tmppass.bc $1.bc

#$LPATH/llvm-instrument/llvm-bin/bin/llc tmppass.bc

#$LPATH/llvm-instrument/llvm-bin/bin/clang tmppass.s -o $1.out -L$LPATH/llvm-instrument/llvm-bin/lib/ -lmonitor -lpthread -lm

