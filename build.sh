#!/bin/bash

project_dir=$(cd "$(dirname "$0")"; 'pwd')
echo "project_dir=$project_dir"
zero_mcu_home=$(cd "$project_dir/.."; 'pwd')
zero_mcu_home=$zero_mcu_home/zero-mcu
build_dir=$project_dir/build
build_type="Debug"

sync_zero_path()
{
    echo "sync zero path ..."
    if [ -d $ZERO_MCU_HOME/.repo ]; then
        echo "Enter $ZERO_MCU_HOME"
        pushd $ZERO_MCU_HOME
        repo sync
        popd
        echo "Exit"
    else
        sh scripts/install.sh --install $zero_mcu_home
    fi
}

. ~/.bashrc
sync_zero_path
. $ZERO_MCU_HOME/scripts/env.sh

build_project()
{
    echo "build project"
    mkdir -p $build_dir
    cd $build_dir

    if [ $build_type = "Debug" ]; then
        cmake -DCMAKE_BUILD_TYPE="Debug" -DZERO_SDK_PATH=$ZERO_SDK_PATH -DCMAKE_TOOLCHAIN_FILE=$ZERO_STM32_CMAKE_TOOLCHAIN_FILE -DSTM32Cube_DIR=${ZERO_3RD_PATH}/STM32Cube_FW_F1_V1.7.0 -DTOOLCHAIN_PREFIX=$ZERO_GCC_TOOLCHAIN_PREFIX $project_dir
    elif [ $build_type = "Release" ]; then
        cmake -DCMAKE_BUILD_TYPE="Release" -DZERO_SDK_PATH=$ZERO_SDK_PATH -DCMAKE_TOOLCHAIN_FILE=$ZERO_STM32_CMAKE_TOOLCHAIN_FILE -DSTM32Cube_DIR=${ZERO_3RD_PATH}/STM32Cube_FW_F1_V1.7.0 -DTOOLCHAIN_PREFIX=$ZERO_GCC_TOOLCHAIN_PREFIX $project_dir
    fi
    make
}


clean_project()
{
    echo "clean project"
    rm -rf $build_dir
    exit 1
}

Usage()
{
    echo "----------------------------------------
Usage:
    sh build.sh [debug|release] --sync
    sh build.sh clean
------------------------------------------"
    exit 1
}

if [ "$#" -eq 1 ] && [ "$1" = "-h" ]; then
    Usage
elif [ "$#" -ge 1 ]; then
    if [ "$1" != "--sync" ]; then
        if [ "$1" == "debug" ]; then
            build_type="Debug"
        elif [ "$1" == "release" ]; then
            build_type=$1
        else
            Usage
        fi
    fi

    for arg in $*
    do
        if [ $arg = "--sync" ]; then
            sync_type="sync"
        fi
    done
fi

build_project



