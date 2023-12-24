#! /bin/sh
# Script to flash imagess via fastboot
#

# This script can only be run in LINUX OS! 
# NOT SUPPORT to flash imagess by fastboot IN MAC OS!!!

sudo ../../../fastboot flash tee ../../../../../../prebuild/images/light-fm-a/tee.evb_light.ext4
