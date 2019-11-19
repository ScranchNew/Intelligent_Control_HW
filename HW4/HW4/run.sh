#!/bin/bash

# Compile & run
g++ -std=c++11 QEAtest.cpp HStopwatch.cpp -o QEA.o
./QEA.o


g++ -std=c++11 GA_Dejong.c HStopwatch.cpp -o GA.o
./GA.o

# Plotting
#echo 'Plotting...'
#matlab -nosplash -nodesktop -r "run('matlab/plotting.m');exit" | tail -n +11