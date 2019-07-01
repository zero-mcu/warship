#!/bin/bash

INSTALL_PATH=~/Development/workspace
ZERO_SDK_NAME=zero-mcu

BACKUP_PATH=~/bakcup
BASHRC_PATH=~/

sync_type="none"

Usage()
{
cat <<EOF
$0 [name] [--install install_path] [--version version]
    name: project name.
    install_path: install code to the path.
    version: zero mcu SDK version.
example:
    $0 zero-mcu --install ~/demo --version 0.1
EOF
exit 0
}

VERSION=0.1

if [ $1 = "-h" ] || [ $1 = "?" ] || [ $1 = "--help" ]; then
    Usage
fi

if [ $# -ge 1 ]; then
    if [ $1 != '--install' ] && [ $1 != '--version' ]; then
        ZERO_SDK_NAME=$1
    fi
fi

param_type="none"
for arg in $*
do
    if [ $arg = '--install' ]; then
        param_type="install"
    elif [ $arg = '--version' ]; then
        param_type="version"
    else
        if [ $param_type = "install" ]; then
            INSTALL_PATH=$arg
        elif [ $param_type = "version" ]; then
            VERSION=$arg
        fi
        param_type="none"
    fi
done

ZERO_MCU_HOME=$INSTALL_PATH/$ZERO_SDK_NAME

echo "install zero-mcu to $INSTALL"
echo "version: $VERSION"

SyncRepo()
{
    if [ ! -d $ZERO_MCU_HOME ]; then
        echo "create $ZERO_MCU_HOME"
        mkdir -p $ZERO_MCU_HOME
    else
        echo "$ZERO_MCU_HOME already exists, clean the folder first."
        rm -rf $ZERO_MCU_HOME/*
        rm -rf $ZERO_MCU_HOME/.repo
    fi

    cd $ZERO_MCU_HOME

    repo init -u https://github.com/zero-mcu/manifest.git \
            --repo-url=https://github.com/ArnoYuan/git-repo.git

    repo sync
}

GenerateEnv()
{
    if [ -f $BACKUP_PATH/.bashrc ]; then
        echo "Environment variable has already been generated, Please check /etc/profile.bak\
        or $BACKUP_PATH/.bashrc.bak, maybe has exist!"
        exit 1
    else
        if [ ! -d $BACKUP_PATH ]; then
            mkdir -p $BACKUP_PATH
        fi
        cp $BASHRC_PATH/.bashrc $BACKUP_PATH/ >/dev/null 2>&1
        mv $BASHRC_PATH/.bashrc $BASHRC_PATH/.bashrc.bak > /dev/null 2>&1

        sed -i 's/export ZERO_MCU_HOME=.*//g' $BACKUP_PATH/.bashrc
        # remove last '\n'
        line=`sed -n '$=' $BACKUP_PATH/.bashrc`
        line=`expr $line - 1`
        sed -i $line'{N;s/^\n//}' $BACKUP_PATH/.bashrc
        line=`sed -n '$=' $BACKUP_PATH/.bashrc`
        line=`expr $line - 1`
        sed -i $line'{N;s/^\n//}' $BACKUP_PATH/.bashrc

        echo >>$BACKUP_PATH/.bashrc
        echo "export ZERO_MCU_HOME=$ZERO_MCU_HOME" >> $BACKUP_PATH/.bashrc
        mv $BACKUP_PATH/.bashrc $BASHRC_PATH/
    fi
}

GenerateEnv
SyncRepo
