#!/bin/bash
rm -rf  build/i386
mkdir -p build/i386
mkdir -p libs/i386
cd build/i386
cmake -DDEBUG=OFF -DCMAKE_TOOLCHAIN_FILE=../../ios.toolchain.cmake -DENABLE_ARC=1 -DIOS_PLATFORM=SIMULATOR ../../
make
mv -f data-reporter.framework ../../libs/i386/
