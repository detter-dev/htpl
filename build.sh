#!/bin/bash

if [ ! -d "build" ]; then
    mkdir build
fi

gcc -std=c99 -Og -g -o build/htpl src/htpl.c  
