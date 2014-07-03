#!/bin/bash

# ######################################
# Running a subset of the NAS programs
# ######################################

echo "Running BT..."
./main.py ./bt.S.x.out

echo "Running CG..."
./main.py ./cg.S.x.out

echo "Running EP..."
./main.py ./ep.S.x.out

echo "===== DONE ====="