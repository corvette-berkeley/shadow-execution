#!/bin/bash

# ###################################
# Shell script to remove output files
# Use: ./compile.sh driver_name .
# ####################################

#export GSL_PATH=/home/eecs/nacuong/tools/gsl-1.15-float-roots/
export GSL_PATH=/home/eecs/nacuong/tools/gsl-1.15/

clang -emit-llvm -c -g $2/$1.c -I$GSL_PATH -o $2/temp_$1.bc
clang -emit-llvm -c -g ../../logging/timers.c -o $2/timers.bc
clang -emit-llvm -c -g ../../logging/cov_checker.c -o $2/cov_checker.bc
clang -emit-llvm -c -g ../../logging/cov_serializer.c -o $2/cov_serializer.bc
clang -emit-llvm -c -g ../../logging/cov_log.c -o $2/cov_log.bc
clang -emit-llvm -c -g ../../logging/cov_rand.c -o $2/cov_rand.bc
llvm-link -o $2/$1.bc $2/temp_$1.bc $2/timers.bc $2/cov_checker.bc $2/cov_serializer.bc $2/cov_log.bc $2/cov_rand.bc $GSL_PATH/libgsl-3.0.so.bc $GSL_PATH/libgslcblas-3.0.so.bc





