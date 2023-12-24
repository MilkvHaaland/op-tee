#!/bin/bash

version=1.0
yaml_file=""
algorithm=""
encryption=""
image_sbmeta_path=sbmeta
ext4tool_path=./bin2ext4/bin2ext4.sh
signtool_path=./imagesign.sh
sbmetatool_path=sbmeta.py # python script path
pack_filepath=./bin2ext4/sbmeta.bin # sbmeta image path to be copied to sbmeta.ext4
output=./bin2ext4/sbmeta.ext4 # sbmeta.ext4 path
py_result="" # python sbmeta.py output

function help {
    echo "The utility is designed for generate sbmeta.ext4"
    echo "usage: ./sbmetagen.sh <ia|sm|nor> <enc|nor> [option]"
    echo ""
    echo "option:"
    echo "-h, --help    display helful information"
    echo "-f, --file    sbmeta configuration yaml file path. None by default"
    echo "-v, --version version of sbmeta image. 1.0 by default"
    echo "-o, --output  sbmeta.ext4 output path. ./bin2ext4/sbmeta.ext4 by default"
    echo ""
    echo "e.g."
    echo "./sbmetagen.sh ia nor -f ./sbmeta.yaml -v 1.0 -o ./bin2ext4/sbmeta.ext4"
    echo ""
    echo ""
    exit
}

function check_version {
    if [[ ! $1 =~ ^[0-9]+\.[0-9]+$ ]]; then
        echo "invalid version"
        return 1
    fi

    # split x.y
    IFS='.' read -r x y <<< "$1"

    # check version
    if [[ $x -ge 0 && $x -le 255 && $y -ge 0 && $y -le 255 ]]; then
        return 0
    else
        echo "invalid version"
        return 1
    fi
}

function sbmeta_gen {
    echo "step 1: generate sbmeta.bin by configuration yaml file"
    py_result=$(python $sbmetatool_path $yaml_file)
    if [ $? != 0 ]; then
        echo "$py_result"
        echo "failed"
        exit
    fi
    echo "$py_result"

    check_version $version
    if [ $? != 0 ]; then
        echo "failed"
        exit
    fi

    echo "step 2: sign sbmeta.bin"
    $signtool_path $algorithm $encryption sbmeta $version
    if [ $? != 0 ]; then
        echo "failed"
        exit
    fi

    local signed_filepath=$image_sbmeta_path/signed_${algorithm}_${encryption}_v${version}_sbmeta.bin
    echo "$signed_filepath"
    if [[ ! -f $signed_filepath ]]; then
        echo "failed"
        exit
    fi

    mv $signed_filepath $pack_filepath
    if [ $? != 0 ]; then
        echo "failed"
        exit
    fi

    echo "step 3: copy signed sbmeta.bin to $pack_filepath"
    $ext4tool_path $output $pack_filepath
    if [ $? != 0 ]; then
        echo "failed"
        exit
    fi
}

# process commandline option
function main {
    if [ $# -lt 2 ]; then
        echo "Missing required paramters"
        exit
    fi

    if [[ $1 != "ia" && $1 != "sm" && $1 != "nor" ]]; then
        echo "invalid parameter value"
        help
    fi

    algorithm=$1
    shift

    if [[ $1 != "enc" && $1 != "nor" ]]; then
        echo "invalid parameter value"
        help
    fi

    encryption=$1
    shift

    while [[ "$1" != "" ]]; do
        case $1 in
            -h | --help )
                help
                exit
                ;;
            -v | --version )
                shift
                version=$1
                ;;
            -f | --file )
                shift
                file=$1
                ;;
            -o | --output )
                shift
                output=$1
                ;;
            * )
                help
                exit 1
                ;;
        esac
        shift
    done

    sbmeta_gen
    if [[ $? -eq 0 ]]; then
        echo "sbmeta.ext4 generates successfully"
        ir=$(sed -n 's/.*\(Signed file: \)\(.*\)/\2/p' <<< "$py_result")
        if [ ! -z  "$ir" ]; then
            echo "sbmeta.py also signed following files:"
            echo "$ir"
        fi
    else
        exit -1
    fi
}

if [[ $1 == "-h" || $1 == "--help" ]]; then
    help
    exit
fi
main $@
