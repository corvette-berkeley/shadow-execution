#!/bin/bash

# #####################################################
# Running original and instrumented NAS programs in $1
# #####################################################

while read program           
do           
    echo $program
    ./main.py $program.out2
    ./main.py $program.out
done < $1           
