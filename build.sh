#!/bin/bash

if [ ! -d "build" ]; then
    mkdir build
fi

pushd build
gcc -std=c99 ../src/htlp.c 
popd
