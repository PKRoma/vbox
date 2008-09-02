#!/bin/bash
## @file
# For development.
#

#
# Copyright (C) 2006-2007 Sun Microsystems, Inc.
#
# Sun Microsystems, Inc. confidential
# All rights reserved
#

DRVNAME="VBoxNetFlt.kext"
BUNDLE="org.virtualbox.kext.VBoxNetFlt"

DEP_DRVNAME="VBoxDrv.kext"
DEP_BUNDLE="org.virtualbox.kext.VBoxDrv"


DIR=`dirname "$0"`
DIR=`cd "$DIR" && pwd`
DEP_DIR="$DIR/$DEP_DRVNAME"
DIR="$DIR/$DRVNAME"
if [ ! -d "$DIR" ]; then
    echo "Cannot find $DIR or it's not a directory..."
    exit 1;
fi
if [ ! -d "$DEP_DIR" ]; then
    echo "Cannot find $DEP_DIR or it's not a directory... (dependency)"
    exit 1;
fi
if [ -n "$*" ]; then
  OPTS="$*"
else
  OPTS="-t"
fi

trap "sudo chown -R `whoami` $DIR $DEP_DIR; exit 1" INT

# Try unload any existing instance first.
LOADED=`kextstat -b $BUNDLE -l`
if test -n "$LOADED"; then
    echo "loadnetflt.sh: Unloading $BUNDLE..."
    sudo kextunload -v 6 -b $BUNDLE
    LOADED=`kextstat -b $BUNDLE -l`
    if test -n "$LOADED"; then
        echo "loadnetflt: failed to unload $BUNDLE, see above..."
        exit 1;
    fi
    echo "loadnetflt: Successfully unloaded $BUNDLE"
fi

set -e

# Copy the .kext to the symbols directory and tweak the kextload options.
if test -n "$VBOX_DARWIN_SYMS"; then
    echo "loadnetflt.sh: copying the extension the symbol area..."
    rm -Rf "$VBOX_DARWIN_SYMS/$DRVNAME"
    mkdir -p "$VBOX_DARWIN_SYMS"
    cp -R "$DIR" "$VBOX_DARWIN_SYMS/"
    OPTS="$OPTS -s $VBOX_DARWIN_SYMS/ "
    sync
fi

# On smbfs, this might succeed just fine but make no actual changes,
# so we might have to temporarily copy the driver to a local directory.
sudo chown -R root:wheel "$DIR" "$DEP_DIR"
OWNER=`/usr/bin/stat -f "%u" "$DIR"`
if test "$OWNER" -ne 0; then
    TMP_DIR=/tmp/loadusb.tmp
    echo "loadnetflt.sh: chown didn't work on $DIR, using temp location $TMP_DIR/$DRVNAME"

    # clean up first (no sudo rm)
    if test -e "$TMP_DIR"; then
        sudo chown -R `whoami` "$TMP_DIR"
        rm -Rf "$TMP_DIR"
    fi

    # make a copy and switch over DIR
    mkdir -p "$TMP_DIR/"
    cp -Rp "$DIR" "$TMP_DIR/"
    DIR="$TMP_DIR/$DRVNAME"

    # load.sh puts it here.
    DEP_DIR="/tmp/loaddrv.tmp/$DEP_DRVNAME"

    # retry
    sudo chown -R root:wheel "$DIR" "$DEP_DIR"
fi

sudo chmod -R o-rwx "$DIR"
sync
echo "loadnetflt: loading $DIR... (kextload $OPTS \"$DIR\")"
sudo kextload $OPTS -d "$DEP_DIR" "$DIR"
sync
sudo chown -R `whoami` "$DIR" "$DEP_DIR"
kextstat | grep org.virtualbox.kext

