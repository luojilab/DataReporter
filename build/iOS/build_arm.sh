#!/bin/bash
rm -rf build/arm
mkdir -p build/arm
mkdir -p libs/arm
cd build/arm
cmake -DDEBUG=OFF -DCMAKE_TOOLCHAIN_FILE=../../ios.toolchain.cmake -DENABLE_ARC=1 -DIOS_PLATFORM=OS ../../
make
mv -rf data-reporter.framework ../../libs/arm/
