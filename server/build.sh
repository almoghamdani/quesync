#!/bin/bash

# Create the build folder
mkdir -p build

# Build the makefile
cd build
cmake -DUSE32bt=false ..

make

cd ..