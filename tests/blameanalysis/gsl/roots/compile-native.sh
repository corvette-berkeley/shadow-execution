#!/bin/bash

# ###################################
# Shell script to remove output files
# Use: ./compile.sh driver_name .
# ####################################

export GSL_PATH=/home/eecs/nacuong/tools/gsl-1.15

gcc -O2 -lm -c $2/$1.c -I$GSL_PATH 
gcc -O2 -lm -c $2/demo_fn.c -I$GSL_PATH 
gcc -O2 -lm -c ../logging/timers.c
gcc -O2 -lm -o $2/$1.out timers.o $2/$1.o $2/demo_fn.o $GSL_PATH/libgsl.so $GSL_PATH/libgslcblas.so
