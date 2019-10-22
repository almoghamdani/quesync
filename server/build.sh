#!/bin/bash

# Create the build folder
mkdir -p build

# Build the ninja build
cd build
cmake -G Ninja ..

# Compile the server
cmake --build .

cd ..