#!/bin/bash

# Author : Matthew Silva

file="ThreadRacer"

if [ -f $file ] ; then
    rm $file
fi

gcc -Wall userThreadRacer.c -lm -lpthread -lncurses -g -o ThreadRacer

./ThreadRacer $1