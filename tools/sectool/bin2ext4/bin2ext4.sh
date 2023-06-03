#!/bin/sh

version=v1.0

echo "bin2ext4 utility version ${version}"
echo ""
mkdir tmp_root
dd if=/dev/zero of=$1 bs=1M count=8
mkfs.ext4 $1 -O ^metadata_csum
sudo mount -t ext4 $1  tmp_root
sudo cp $2 ./tmp_root
sync
sudo umount ./tmp_root
rm -fr tmp_root
