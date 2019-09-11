#!/bin/bash
./build_x86_64.sh
./build_os_arch.sh
mkdir -p libs/all
lipo -create libs/os/libdata-reporter.a libs/x86_64/libdata-reporter.a -output libs/all/libdata-reporter.a
