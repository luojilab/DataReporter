#!/bin/bash
rm -rf build/arm
mkdir -p build/arm
mkdir -p libs/arm
cd build/arm
cmake -DDEBUG=OFF -DNDEBUG=1 -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../../ios.toolchain.cmake -DDEPLOYMENT_TARGET="8.0" -DENABLE_ARC=1 -DPLATFORM=OS ../../
make -j4
mv -f DataReporter.framework ../../libs/arm/
