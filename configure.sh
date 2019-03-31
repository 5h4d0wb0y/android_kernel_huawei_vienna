#!/bin/bash

# Check if toolchain exists
if [ ! -d $HOME/toolchain ]; then
    git clone https://android.googlesource.com/platform/prebuilts/gcc/linux-x86/aarch64/aarch64-linux-android-4.9 -b nougat-release $HOME/toolchain
fi
