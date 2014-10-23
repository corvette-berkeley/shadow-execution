#!/bin/bash

export CC=$LLVM_BIN_PATH"/clang -use-gold-plugin"
export LDFLAGS="-L"$INSTRUMENTOR_LIB_PATH" -L"$BLAMEANALYSIS_LIB_PATH" -L"$GLOG_LIB_PATH" -L"$GPERFTOOLS_LIB_PATH" -L"$UNWIND_LIB_PATH" -L"$FPPASS_LIB_PATH

name=$(basename $1 .c)

# Compile into bitcode
$LLVM_BIN_PATH/clang -emit-llvm -g -Xclang -dwarf-column-info -pg -c $name.c -o $name.bc

# Instrument the bitcode
$LLVM_BIN_PATH/opt -load $FPPASS_LIB_PATH/FPPass.so -fppass -f -o $name-fp.bc $name.bc

# Create executable
# TODO: DOES NOT WORK
$CC $name-fp.bc -o $name.out $LDFLAGS -lba2 -lpthread -lm -lrt -lgmp -lglog  -ltcmalloc

# Clean temporary files
rm $name.bc
rm $name-fp.bc
