#!/bin/bash
mkdir build
cd build
cmake ../../../src -DDEBUG=OFF -DCMAKE_TOOLCHAIN_FILE=../ios.toolchain.cmake -DIOS_PLATFORM=SIMULATOR64
make