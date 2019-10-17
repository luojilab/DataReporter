#!/bin/bash
mkdir -p build/i386
mkdir -p libs/i386
cd build/i386
cmake -DDEBUG=OFF -DCMAKE_TOOLCHAIN_FILE=../../ios.toolchain.cmake -DIOS_PLATFORM=SIMULATOR ../../
make
mv libdata-reporter.a ../../libs/i386/
