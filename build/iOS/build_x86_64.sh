#!/bin/bash
mkdir -p build/x86_64
mkdir -p libs/x86_64
cd build/x86_64
cmake -DDEBUG=OFF -DCMAKE_TOOLCHAIN_FILE=../ios.toolchain.cmake -DIOS_PLATFORM=SIMULATOR64 ../
make
mv libdata-reporter.a ../../libs/x86_64/