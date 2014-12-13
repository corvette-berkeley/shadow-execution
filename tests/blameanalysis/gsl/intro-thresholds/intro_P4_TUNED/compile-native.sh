#!/bin/bash

# ###################################
# Shell script to remove output files
# Use: ./compile.sh driver_name .
# ####################################

export GSL_PATH=/home/eecs/nacuong/tools/gsl-1.15-float

gcc -c -lm -O2 $2/$1.c -I$GSL_PATH 
gcc -c -lm -O2 ../../logging/timers.c
gcc -c -lm -O2 ../../logging/cov_checker.c
gcc -c -lm -O2 ../../logging/cov_serializer.c
gcc -c -lm -O2 ../../logging/cov_log.c
gcc -c -lm -O2 ../../logging/cov_rand.c
gcc -lm -O2 -o $2/$1-native.out $2/$1.o $2/timers.o $2/cov_checker.o $2/cov_serializer.o $2/cov_log.o $2/cov_rand.o $GSL_PATH/libgsl.so $GSL_PATH/libgslcblas.so
