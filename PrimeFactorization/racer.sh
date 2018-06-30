#!/bin/bash

# Author : Matthew Silva

file="ThreadRacer"

if [ -f $file ] ; then
    rm $file
fi

gcc userThreadRacer.c -o ThreadRacer -lpthread -g -lncurses

./ThreadRacer $1 $2 $3