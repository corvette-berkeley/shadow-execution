#!/bin/bash

# ###################################
# Shell script to remove output files
# Use: ./compile.sh driver_name .
# ####################################

clang -emit-llvm -c -g $2/$1.c -I$GSL_PATH -o $2/temp_$1.bc
clang -emit-llvm -c -g $2/demo_fn.c -I$GSL_PATH -o $2/demo_fn.bc
llvm-link -o $2/$1.bc $2/temp_$1.bc $2/demo_fn.bc $GSL_PATH/libgsl.so.bc $GSL_PATH/libgslcblas.so.bc





