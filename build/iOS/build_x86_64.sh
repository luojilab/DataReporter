#!/bin/bash
mkdir build
cd build
cmake -DDEBUG=OFF -DCMAKE_TOOLCHAIN_FILE=../ios.toolchain.cmake -DIOS_PLATFORM=SIMULATOR64 ../../../src
make