#!/bin/bash


while read program           
do           
    echo $program
    ./main.py $program.out
done < tests.txt           
