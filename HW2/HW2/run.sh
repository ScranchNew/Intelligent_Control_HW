#!/bin/bash

# Compile & run
g++ -std=c++11 main.cpp -o main.o
./main.o

# Plotting
echo 'Plotting...'
matlab -nosplash -nodesktop -r "run('matlab/plotting.m');exit" | tail -n +11