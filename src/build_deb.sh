#!/bin/bash

WORKDIR=$(pwd)
SCR_TOP=$(cd $(dirname $0); pwd)
BIN_DIR=$SCR_TOP

if [ ! -d "$BIN_DIR" ]; then
    echo "BinDir $BIN_DIR is not exist or not a dir!"
    exit 1
fi

# Usage: UpdateControlValue <Key> <Value>
function UpdateControlValue()
{
    local Key=$1
    local Value=$2
    if [ $# -ne 2 ]; then
        echo "Usage: UpdateControlValue <Key> <Value>"
        return 1
    fi

    local Line=$(grep -n "^${Key}:" $CONTROL_FILE | awk -F: '{print $1}' | sed -n 1p)
    if [ -n "${Line}" ]; then
        if ! sed -i "${Line}c${Key}:\ ${Value}" $CONTROL_FILE; then
            echo "Update Value of Key: $Key in file $CONTROL_FILE failed!"
            return 1;
        fi
    fi
}

BUILD_DIR=$WORKDIR/.build_deb
CONTROL_FILE=$BUILD_DIR/DEBIAN/control

# Copy files
mkdir -p $BUILD_DIR/DEBIAN
cp -at $BUILD_DIR/DEBIAN $SCR_TOP/debian/*

DEST_DIR=/opt/test-utils

mkdir -p $BUILD_DIR/$DEST_DIR/bin
cp -at $BUILD_DIR/$DEST_DIR/bin $BIN_DIR/ScreenTestKit

mkdir -p $BUILD_DIR/usr/share/applications
cp -at $BUILD_DIR/usr/share/applications $SCR_TOP/ScreenTestKit.desktop

# Update control file
PKG_NAME=$(grep "Package" $CONTROL_FILE | awk '{print $2}')
PKG_VER=$(grep '^VERSION' ScreenTestKit.pro | awk '{print $3}')
PKG_ARCH=$(dpkg --print-architecture)

UpdateControlValue "Version" "$PKG_VER"
UpdateControlValue "Architecture" "$PKG_ARCH"
[ -f Maintainer ] && UpdateControlValue "Maintainer" "$(cat Maintainer)"
[ -f Homepage ] && UpdateControlValue "Homepage" "$(cat Homepage)"

# Build Package
fakeroot dpkg-deb -b $BUILD_DIR ${PKG_NAME}_${PKG_VER}_${PKG_ARCH}.deb || exit $?
rm -rf $BUILD_DIR
