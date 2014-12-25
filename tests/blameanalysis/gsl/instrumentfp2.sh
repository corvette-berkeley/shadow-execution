#!/bin/bash

export CC=$LLVM_BIN_PATH"/clang"
export LDFLAGS="-L"$INSTRUMENTOR_LIB_PATH" -L"$BLAMEANALYSIS_LIB_PATH"" 
# -L"$GLOG_LIB_PATH" -L"$GPERFTOOLS_LIB_PATH" -L"$UNWIND_LIB_PATH" -L"$FPPASS_LIB_PATH

name=$1

# Instrument the bitcode
$LLVM_BIN_PATH/opt -load $FPPASS_LIB_PATH/FPPass.so -fppass -f -o $name-fp.bc $name.bc -exclude exclude.txt

# Create executable for instrumented bitcode file
$CC $name-fp.bc -o $name.out $LDFLAGS -lba3 -lpthread -lm -lrt -lgmp 

# Create executable for original bitcode file
$CC $name.bc -o $name.out2 $LDFLAGS -lpthread -lm -lrt -lgmp 

# Clean temporary files
#rm $name-fp.bc
