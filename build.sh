#!/bin/bash

if [ ! -d "build" ]; then
    mkdir build
fi

pushd build
gcc ../src/htlp.c 
popd
