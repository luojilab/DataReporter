#!/bin/bash
rm -rf build
./build_i386.sh
./build_x86_64.sh
./build_arm.sh
mkdir -p libs/all/DataReporter.framework
lipo -create libs/arm/DataReporter.framework/DataReporter libs/i386/DataReporter.framework/DataReporter  -output libs/all/DataReporter.framework/DataReporter
cp -rf libs/arm/DataReporter.framework/Headers libs/arm/DataReporter.framework/*.plist libs/all/DataReporter.framework
