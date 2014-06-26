#!/bin/bash

# ###################################
# Instrumenting all GSL programs
# ####################################

echo "Instrumenting bessel..."
cd bessel
../instrument.sh bessel .
cd ..

echo "Instrumenting blas..."
cd blas
../instrument.sh blas .
cd ..

echo "Instrumenting gaussian..."
cd gaussian
../instrument.sh gaussian .
cd ..

echo "Instrumenting polyroots..."
cd polyroots
../instrument.sh polyroots .
cd ..

echo "Instrumenting roots..."
cd roots
../instrument.sh roots .
cd ..

echo "Instrumenting sum..."
cd sum
../instrument.sh sum .
cd ..




echo "===== DONE ====="