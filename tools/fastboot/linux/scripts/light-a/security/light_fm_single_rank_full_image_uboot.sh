#! /bin/sh
# Script to flash imagess via fastboot
#

# This script can only be run in LINUX OS! 

sudo ../../../fastboot flash uboot ../../../../../../prebuild/images/light-fm-a/u-boot-with-spl.bin
sudo ../../../fastboot flash tf ../../../../../../prebuild/images/light-fm-a/tf.ext4
sudo ../../../fastboot flash tee ../../../../../../prebuild/images/light-fm-a/tee.ext4
sudo ../../../fastboot flash boot ../../../../../../prebuild/images/light-fm-a/boot.ext4
sudo ../../../fastboot flash root ../../../../../../prebuild/images/light-fm-a/rootfs.linux.ext4
