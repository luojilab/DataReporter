#!/bin/bash
./build_i386.sh
./build_x86_64.sh
./build_arm.sh
mkdir -p libs/all
lipo -create libs/arm/libdata-reporter.a libs/i386/libdata-reporter.a libs/x86_64/libdata-reporter.a -output libs/all/libdata-reporter.a
