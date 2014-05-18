#!/bin/bash

# ###################################
# Shell script to remove output files
# Use: ./compile.sh driver_name .
# ####################################

loggingPath=$CORVETTE_PATH"/logging"

clang -emit-llvm -c $2/$1.c -I$GSL_PATH/include -o $2/temp_$1.bc
clang -emit-llvm -c $2/demo_fn.c -I$GSL_PATH/include -o $2/demo_fn.bc

llvm-link -o $2/$1.bc $2/temp_$1.bc $2/demo_fn.bc $CORVETTE_PATH/benchmarks/gsl/libgsl-LLVM3.3.so.bc $CORVETTE_PATH/benchmarks/gsl/libgslcblas-LLVM3.3.so.bc





