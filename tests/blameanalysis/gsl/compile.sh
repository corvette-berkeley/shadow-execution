#!/bin/bash

# ###################################
# Shell script to remove output files
# Use: ./compile.sh driver_name .
# ####################################

clang -emit-llvm -c -g $2/$1.c -I$GSL_PATH/include -o $2/temp_$1.bc
llvm-link -o $2/$1.bc $2/temp_$1.bc $GSL_LIB_PATH/libgsl.so.bc $GSL_LIB_PATH/libgslcblas.so.bc

