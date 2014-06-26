#!/bin/bash

# ######################################
# Instrumenting a subset of the NAS programs
# ######################################

echo "Instrumenting BT..."
./instrument.sh bt.S.x .

echo "Instrumenting CG..."
./instrument.sh cg.S.x .

echo "Instrumenting EP..."
./instrument.sh ep.S.x .

echo "===== DONE ====="