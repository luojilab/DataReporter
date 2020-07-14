#!/bin/bash
rm -rf build
./build_i386.sh
./build_x86_64.sh
./build_arm.sh
mkdir -p libs/all/data-reporter.framework
lipo -create libs/arm/data-reporter.framework/data-reporter libs/i386/data-reporter.framework/data-reporter libs/x86_64/data-reporter.framework/data-reporter -output libs/all/data-reporter.framework/data-reporter
cp -rf libs/arm/data-reporter.framework/Headers libs/all/data-reporter.framework