#!/bin/bash

# ###################################
# Running all GSL programs
# ####################################

echo "Running bessel..."
bessel/bessel.out bessel/input1.txt

echo "Running blas..."
blas/blas.out

echo "Running gaussian..."
gaussian/gaussian.out gaussian/input1.txt

echo "Running polyroots..."
polyroots/polyroots.out polyroots/input1.txt

echo "Running roots..."
roots/roots.out roots/input1.txt

echo "Running sum..."
sum/sum.out

echo "===== DONE ====="