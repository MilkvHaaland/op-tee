#! /bin/sh
# Script to flash imagess via fastboot
#

# This script can only be run in LINUX OS! 
# NOT SUPPORT to flash imagess by fastboot IN MAC OS!!!

sudo ../../../fastboot flash ram ../../../../../../prebuild/images/light-fm-a/u-boot-with-spl.bin
sudo ../../../fastboot reboot
sleep 5
sudo ../../../fastboot flash uboot ../../../../../../prebuild/images/light-fm-a/u-boot-with-spl.bin
