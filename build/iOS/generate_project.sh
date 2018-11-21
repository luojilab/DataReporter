#!/bin/bash
mkdir build
cd build
cmake ../../../src -DCMAKE_TOOLCHAIN_FILE=../ios.toolchain.cmake  -DCMAKE_IOS_DEVELOPER_ROOT=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/ -DCMAKE_IOS_SDK_ROOT=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk/  -G Xcode .