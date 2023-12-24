#!/bin/bash

LA_TEE_PATH="../../prebuild/images/light-fm-a/tee.evb_light.ext4"
LA_SBMETA_PATH="../../prebuild/images/light-fm-a/sbmeta.ext4"
LB_TEE_PATH="../../prebuild/images/light-fm-b/tee.light_b.ext4"
LB_SBMETA_PATH="../../prebuild/images/light-fm-b/sbmeta.ext4"
SOM_TEE_PATH="../../prebuild/images/light_lpi4a/tee.lichee_pi_4a.ext4"
SOM_SBMETA_PATH="../../prebuild/images/light_lpi4a/sbmeta.ext4"

function umount_tee {
    if [[ -d "temp" ]]; then
        rm -rf temp
    fi
    mkdir temp
    sudo mount $1 temp
    cp temp/trust_firmware.bin tf/
    cp temp/tee.bin tee/
    sudo umount temp
}

function upd_sbmeta {
    ./sbmetagen.sh ia nor -o $1
    if [[ $? != 0 ]]; then
        return -1
    fi
}

function update_LA {
    if [[ ! -f $LA_TEE_PATH ]]; then
        echo "tee.evb_light.ext4 doesn't exist"
        return -1
    fi
    umount_tee $LA_TEE_PATH
    dir=$(dirname $LA_SBMETA_PATH)
    if [ ! -d "$dir" ]; then
        echo "LA SBMETA path is invalid"
        return -1
    fi
    output=$(upd_sbmeta $LA_SBMETA_PATH)
    if [[ $? != 0 ]]; then
        echo "$output"
        return -1
    fi
}

function update_LB {
    if [[ ! -f $LB_TEE_PATH ]]; then
        echo "tee.light_b.ext4 doesn't exist"
        return -1
    fi
    umount_tee $LB_TEE_PATH
    dir=$(dirname $LB_SBMETA_PATH)
    if [ ! -d "$dir" ]; then
        echo "LB SBMETA path is invalid"
        return -1
    fi
    output=$(upd_sbmeta $LB_SBMETA_PATH)
    if [[ $? != 0 ]];then
        echo "$output"
        return -1
    fi
}

function update_SOM {
    if [[ ! -f $SOM_TEE_PATH ]]; then
        echo "tee.lichee_pi_4a.ext4 doesn't exist"
        return -1
    fi
    umount_tee $SOM_TEE_PATH
    dir=$(dirname $SOM_SBMETA_PATH)
    if [ ! -d "$dir" ]; then
        echo "SOM SBMETA path is invalid"
        return -1
    fi
    output=$(upd_sbmeta $SOM_SBMETA_PATH)
    if [[ $? != 0 ]]; then
        echo "$output"
        return -1
    fi
}

function help {
    echo "The utility is designed to update sbmeta.ext4 while tee.xxx.ext4 has been modified"
    echo "usage: ./sbmetaupd.sh [option]"
    echo ""
    echo "option:"
    echo "-h    display helful information"
    echo "-a    update sbmeta.ext4 for LA, LB, SOM"
    echo "-la   update sbmeta.ext4 for LA"
    echo "-lb   update sbmeta.ext4 for LB"
    echo "-som  update sbmeta.ext4 for SOM"
    echo ""
    echo "Note:"
    echo "Please confirm that TEE & SBMETA paths are valid in the script."
    echo "For users, after upload new tee.xxx.ext4, just run the script to update sbmeta.ext4."
    echo "no other relocation operation required"
    exit
}

case $1 in
    -h )
        help
        exit
        ;;
    -a )
        update_LA
        update_LB
        update_SOM
        ;;
    -la )
        update_LA
        ;;
    -lb )
        update_LB
        ;;
    -som )
        update_SOM
        ;;
    * )
        help
        exit 1
        ;;
esac
shift
