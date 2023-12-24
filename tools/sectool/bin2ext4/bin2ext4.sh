#!/bin/sh

version=v1.0

echo "bin2ext4 utility version ${version}"
echo ""
fsname=$1
shift
if [ ! -d "tmp_root" ]; then
        mkdir tmp_root
fi

dd if=/dev/zero of=$fsname bs=1M count=8
if [ ! $? -eq 0 ]; then
        echo "fail"
        exit 1
fi

mkfs.ext4 $fsname -O ^metadata_csum
if [ ! $? -eq 0 ]; then
        echo "fail"
        exit 1
fi

sudo mount -t ext4 $fsname  tmp_root
if [ ! $? -eq 0 ]; then
        echo "fail"
        exit 1
fi

for file in "$@"
do
        sudo cp "$file" ./tmp_root
        if [ ! $? -eq 0 ]; then
                echo "fail"
                exit 1
        fi
done

sync
if [ ! $? -eq 0 ]; then
        echo "fail"
        exit 1
fi

sudo umount ./tmp_root
if [ ! $? -eq 0 ]; then
        echo "fail"
        exit 1
fi

rm -fr tmp_root
if [ ! $? -eq 0 ]; then
        echo "fail"
        exit 1
fi