#!/bin/bash

DATE=`date +%F`
TIME=`date +%s`
PACK_FILE_NAME="$PROJECT_HOME/pkg/lincore-0.1.tar.gz"

rm -rf $PROJECT_HOME/build/work
rm -rf $PACK_FILE_NAME

cd $PROJECT_HOME/build
mkdir work
cd work

cp -R $PROJECT_HOME/build/template lincore

mkdir lincore/bin
mkdir lincore/lib

cp $PROJECT_HOME/third-party/lib/libsigar-amd64-linux.so lincore/lib

if [ "$LINCORE_RELEASE" != "" ]; then
    cp $PROJECT_HOME/bin/lincore lincore/bin/lincored
else
    cp $PROJECT_HOME/bin-dbg/lincore lincore/bin/lincored
fi

if [ ! -d $PROJECT_HOME/pkg ]; then
    mkdir $PROJECT_HOME/pkg
fi

tar cfz $PACK_FILE_NAME lincore

rm -rf $PROJECT_HOME/build/work
