#!/bin/sh

mkdir -p build
echo "removing previous build files"
rm -r build/*
echo "creating build targets"
mkdir -p build/release build/debug

cmake -S . -B build/debug 	-DCMAKE_BUILD_TYPE=DEBUG -DCMAKE_EXPORT_COMPILE_COMMANDS=1
cmake -S . -B build/release -DCMAKE_BUILD_TYPE=RELEASE
