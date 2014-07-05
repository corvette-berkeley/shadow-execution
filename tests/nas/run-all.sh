#!/bin/bash

# ######################################
# Running a subset NAS programs in $1
# ######################################

while read program           
do           
    echo $program
    ./main.py $program.out .
done < $1           

