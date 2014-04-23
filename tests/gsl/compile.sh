#!/bin/bash

# ###################################
# Shell script to remove output files
# Use: ./compile.sh driver_name .
# ####################################

clang -g -emit-llvm -c $2/$1.c -I$GSL_PATH/include -o $2/temp_$1.bc

llvm-link -o $2/$1.bc $2/temp_$1.bc $CORVETTE_PATH/benchmarks/gsl/libgsl.so.bc $CORVETTE_PATH/benchmarks/gsl/libgslcblas.so.bc