#!/bin/bash

project_dir=$(cd "$(dirname "$0")"; 'pwd')
echo "project_dir=$project_dir"
zero_workspace=$(cd "$project_dir/.."; 'pwd')
zero_sdk_name="zero-mcu"
build_dir=$project_dir/build
build_type="Debug"
sync_type="none"


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
        if [ "$1" = "debug" ]; then
            build_type="Debug"
        elif [ "$1" = "release" ]; then
            build_type=$1
        elif [ "$1" = "clean" ]; then
            clean_project
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


sync_zero_path()
{
    echo "sync zero path ..."
    echo "Check zero sdk path: $ZERO_MCU_HOME"
    if [ -d $ZERO_MCU_HOME/.repo ]; then
        if [ $sync_type = "sync" ]; then
            echo "Enter $ZERO_MCU_HOME"
            pushd $ZERO_MCU_HOME
            echo ">>>>>$(pwd)"
            repo sync
            popd
            echo "Exit"
        fi
    else
        echo "No zero sdk found, install..."
        . $HOME/.profile
        if [ ! -d $zero_workspace ]; then
            mkdir -p $zero_workspace
        fi
        echo "Enter $zero_workspace"
        cd $zero_workspace
        if [ ! -d $zero_workspace/manifest ]; then
            echo "No manifest found, clone from https://github.com/zero-mcu/manifest.git"
            git clone https://github.com/zero-mcu/manifest.git
        fi
        echo "Enter $zero_workspace/manifest"
        cd $zero_workspace/manifest
        git pull
        sh install.sh $zero_sdk_name --install $zero_workspace
    fi
}

source $HOME/.profile
sync_zero_path
source $HOME/.profile
source $ZERO_MCU_HOME/scripts/env.sh

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
build_project



