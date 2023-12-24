#! /bin/sh
# Script to flash imagess via fastboot
#

# This script can only be run in LINUX OS! 

sudo ../../../fastboot flash ram ../../../../../../prebuild/images/light-fm-a/u-boot-with-spl.bin
sudo ../../../fastboot reboot
sleep 5
sudo ../../../fastboot flash uboot ../../../../../../prebuild/images/light-fm-a/u-boot-with-spl.bin
sudo ../../../fastboot flash sbmeta ../../../../../../prebuild/images/light-fm-a/sbmeta.ext4
sudo ../../../fastboot flash tee ../../../../../../prebuild/images/light-fm-a/tee.evb_light.ext4
sudo ../../../fastboot flash boot ../../../../../../prebuild/images/light-fm-a/boot.ext4
sudo ../../../fastboot flash root ../../../../../../prebuild/images/light-fm-a/rootfs.linux.ext4
