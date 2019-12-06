#!/bin/bash
rm -rf build/arm
mkdir -p build/arm
mkdir -p libs/arm
cd build/arm
cmake -DDEBUG=OFF -DCMAKE_TOOLCHAIN_FILE=../../ios.toolchain.cmake -DIOS_PLATFORM=OS ../../
make
mv libdata-reporter.a ../../libs/arm/
