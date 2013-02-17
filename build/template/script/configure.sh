#/bin/bash

LinCoreHome=$1

function replaceHOME()
{
    if [ ! -f $1 ]; then
        echo "Expected to find a file $1"
        exit -1
    fi
    sed -i "s|INSTALL_DIR|$LinCoreHome|g" $1
}

if [ "$1" == "" ]; then
    echo "Required one command line argument"
    exit -1
fi

if [ ! -d $LinCoreHome ]; then
    echo "Expected a command line argument with existing directory"
    exit -1
fi

replaceHOME $LinCoreHome/script/start.sh
replaceHOME $LinCoreHome/script/stop.sh

ln -s $LinCoreHome/lib/libsigar-amd64-linux.so $LinCoreHome/lib/libsigar.so

