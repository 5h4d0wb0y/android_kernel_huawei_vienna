Overview
========

This repo is a Android Kernel Source For Huawei P9 Plus (Oreo/MIUI based ROM) downloaded from [Huawei Open Source Release Center](https://consumer.huawei.com/en/opensource/).

Feature
=======

- [Aircrack-ng 802.11 frames inject patch](http://patches.aircrack-ng.org/mac80211.compat08082009.wl_frag+ack_v1.patch) so that can be used for WiFi pentest
- Support The [Kali NetHunter Project](https://github.com/offensive-security/kali-nethunter/wiki)

TODO
====

- [pelya's android-keyboard-gadget patch](https://github.com/pelya/android-keyboard-gadget) so that can be used for HID Attack

Before Build
============

1.Setup Build Environment
-------------------------

```bash
$ sudo apt update
$ sudo install -y git ccache automake flex lzop bison \
gperf build-essential zip curl zlib1g-dev zlib1g-dev:i386 \
g++-multilib python-networkx libxml2-utils bzip2 libbz2-dev \
libbz2-1.0 libghc-bzlib-dev squashfs-tools pngcrush \
schedtool dpkg-dev liblz4-tool make optipng maven libssl-dev \
pwgen libswitch-perl policycoreutils minicom libxml-sax-base-perl \
libxml-simple-perl bc libc6-dev-i386 lib32ncurses5-dev \
x11proto-core-dev libx11-dev lib32z-dev libgl1-mesa-dev xsltproc unzip
```

2.Checkout Kernel Source
------------------------

```bash
$ git clone --depth=1 https://github.com/5h4d0wb0y/android_kernel_huawei_vienna.git
```

3.Download Build Toolchain
--------------------------

```bash
$ ./configure.sh
```

For BUILD MIUI KERNEL
=====================

```bash
$ cd android_kernel_huawei_vienna
$ mkdir out 
$ make O=out mrproper
```
```bash
$ make O=out merge_hi3650_defconfig ARCH=arm64 CROSS_COMPILE=$HOME/toolchain/bin/aarch64-linux-android-
$ make -j$(nproc) O=out 2>&1 ARCH=arm64 CROSS_COMPILE=$HOME/toolchain/bin/aarch64-linux-android-
```

For BUILD Kali NetHunter KERNEL
===============================

```bash
$ chmod +x menuconfig.sh build.sh && ./menuconfig.sh
$ ./build.sh
```

Thanks
======
- Kali NetHunter Project https://github.com/offensive-security/kali-nethunter/wiki
- Aircrack-ng https://github.com/aircrack-ng/aircrack-ng
- pelya https://github.com/pelya/android-keyboard-gadget


