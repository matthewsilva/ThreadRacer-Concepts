#!/bin/bash

# Author : Matthew Silva

file="ThreadRacer"

if [ -f $file ] ; then
    rm $file
fi

gcc -Wall userThreadRacer.c fileIO_TGA.c -lm -lpthread -lncurses -g -o ThreadRacer

./ThreadRacer $1 $2 