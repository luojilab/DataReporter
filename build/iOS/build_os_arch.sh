#!/bin/bash
mkdir -p build/os
mkdir -p libs/os
cd build/os
cmake -DDEBUG=OFF -DCMAKE_TOOLCHAIN_FILE=../../ios.toolchain.cmake -DIOS_PLATFORM=OS ../../
make
mv libdata-reporter.a ../../libs/os/
