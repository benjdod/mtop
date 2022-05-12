#!/bin/sh

# for some reason, cmake doesn't generate compile commands on certain 
# development machines, and this seems to fix it. 

make clean
cmake . -DCMAKE_EXPORT_COMPILE_COMMANDS=1
make
