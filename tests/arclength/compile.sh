#!/bin/bash

#compile into bitcode
$LLVM_BIN_PATH/clang -emit-llvm -g -c $1.c -o $1.bc
$LLVM_BIN_PATH/llvm-dis $1.bc
