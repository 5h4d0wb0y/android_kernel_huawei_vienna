#!/bin/bash

# Check if toolchain64 exists
if [ ! -d toolchain64 ]; then
    #git clone https://android.googlesource.com/platform/prebuilts/gcc/linux-x86/aarch64/aarch64-linux-android-4.9 -b oreo-release toolchain64
    git clone https://android.googlesource.com/platform/prebuilts/gcc/linux-x86/aarch64/aarch64-linux-android-4.9 toolchain64
fi

LOCAL_DIR=`pwd`
export PATH=$PATH:$LOCAL_DIR/toolchain64/bin
export CROSS_COMPILE=aarch64-linux-android-
export ARCH=arm64
export SUBARCH=arm64

if [ ! -d out ]; then
    mkdir out/
else
    rm -rf out/*
fi

make ARCH=arm64 distclean
make O=out ARCH=arm64 CROSS_COMPILE=aarch64-linux-android- kali_defconfig
make O=out ARCH=arm64 CROSS_COMPILE=aarch64-linux-android- -j$(nproc)
