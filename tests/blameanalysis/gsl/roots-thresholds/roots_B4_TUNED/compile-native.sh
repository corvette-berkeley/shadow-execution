#!/bin/bash

# ###################################
# Shell script to remove output files
# Use: ./compile.sh driver_name .
# ####################################

export GSL_PATH=/home/eecs/nacuong/tools/gsl-1.15-float
export LD_LIBRARY_PATH=$GSL_PATH:$LD_LIBRARY_PATH

gcc -c $2/$1.c -I$GSL_PATH 
gcc -c $2/demo_fn.c -I$GSL_PATH 
gcc -o $2/$1.out $2/$1.o $2/demo_fn.o $GSL_PATH/libgsl.so.0 $GSL_PATH/libgslcblas.so.0
