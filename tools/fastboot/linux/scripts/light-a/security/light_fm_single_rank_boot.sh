#! /bin/sh
# Script to flash imagess via fastboot
#

# This script can only be run in LINUX OS! 
# NOT SUPPORT to flash imagess by fastboot IN MAC OS!!!

sudo ../../../fastboot flash uboot ../../../../../../prebuild/images/light-fm-a/u-boot-with-spl.bin
sudo ../../../fastboot flash boot ../../../../../../prebuild/images/light-fm-a/boot.ext4


