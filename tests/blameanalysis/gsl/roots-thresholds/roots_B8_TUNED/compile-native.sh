#!/bin/bash

# ###################################
# Shell script to remove output files
# Use: ./compile.sh driver_name .
# ####################################

export GSL_PATH=/home/eecs/nacuong/tools/gsl-1.15-float-roots

gcc -c -lm -O2 $2/$1.c -I$GSL_PATH 
gcc -c -lm -O2 $2/demo_fn.c -I$GSL_PATH 
gcc -c -lm -O2 ../../logging/timers.c
gcc -o $2/$1.out $2/$1.o $2/demo_fn.o timers.o $GSL_PATH/libgsl.so $GSL_PATH/libgslcblas.so
