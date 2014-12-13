#!/bin/bash

# ###################################
# Shell script to remove output files
# Use: ./compile.sh driver_name .
# ####################################

export GSL_PATH=/home/eecs/nacuong/tools/gsl-1.15

gcc -c -O2 -lm $2/$1.c -I$GSL_PATH 
gcc -c -O2 -lm ../../logging/timers.c
gcc -c -O2 -lm ../../logging/cov_checker.c
gcc -c -O2 -lm ../../logging/cov_serializer.c
gcc -c -O2 -lm ../../logging/cov_log.c
gcc -c -O2 -lm ../../logging/cov_rand.c
gcc -O2 -lm -o $2/$1-native.out $2/$1.o $2/timers.o $2/cov_checker.o $2/cov_serializer.o $2/cov_log.o $2/cov_rand.o $GSL_PATH/libgsl.so $GSL_PATH/libgslcblas.so
