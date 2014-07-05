#!/bin/bash

# ######################################
# Instrumenting NAS programs in $1
# ######################################


while read program           
do           
    echo $program
    ./instrument.sh $program .
done < $1           
