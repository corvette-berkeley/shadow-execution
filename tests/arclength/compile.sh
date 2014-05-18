#!/bin/bash

LPATH=$GOLD_LLVM_BIN

#compile into bitcode
$LPATH/clang -emit-llvm -g -c $1.c -o $1.bc
llvm-dis $1.bc



