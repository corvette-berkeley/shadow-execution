#!/bin/bash

# #####################################
# Shell script to compile GSL programs
# Use: ./compile.sh driver_name .
# #####################################

loggingPath=$CORVETTE_PATH"/logging"

clang -g -emit-llvm -c $2/$1.c -I$GSL_PATH/include -o $2/temp_$1.bc
clang -c -o $2/cov_serializer.o $loggingPath/cov_serializer.c

llvm-link -o $2/$1.bc $2/temp_$1.bc $2/cov_serializer.o $CORVETTE_PATH/benchmarks/gsl/libgsl.so.bc $CORVETTE_PATH/benchmarks/gsl/libgslcblas.so.bc

