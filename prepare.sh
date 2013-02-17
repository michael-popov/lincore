export PROJECT_HOME=`pwd`
export LD_LIBRARY_PATH=$PROJECT_HOME/third-party/lib:$PROJECT_HOME/lib:$LD_LIBRARY_PATH

if [ ! -d work ]; then
    mkdir work
fi

if [ ! -d pkg ]; then
    mkdir pkg
fi

if [ ! -d bin ]; then
    mkdir bin
fi

if [ ! -d lib ]; then
    mkdir lib
fi

if [ ! -d bin-dbg ]; then
    mkdir bin-dbg
fi

if [ ! -d lib-dbg ]; then
    mkdir lib-dbg
fi

if [ ! -d third-party/lib ]; then
    mkdir third-party/lib
fi

rm -rf third-party/lib/libsigar.so
AMD64=`uname -a | grep x86_64 | wc -l`
if [ $AMD64 -eq 1 ]; then
    cp third-party/sigar/libsigar-amd64-linux.so third-party/lib
    ln -s $PROJECT_HOME/third-party/lib/libsigar-amd64-linux.so third-party/lib/libsigar.so
    cp third-party/cdb/dbg-64-libcdbutils.a lib-dbg/libcdbutils.a
    cp third-party/cdb/rel-64-libcdbutils.a lib/libcdbutils.a
else
    cp third-party/sigar/libsigar-x86-linux.so third-party/lib
    ln -s $PROJECT_HOME/third-party/lib/libsigar-x86-linux.so third-party/lib/libsigar.so
    cp third-party/cdb/dbg-32-libcdbutils.a lib-dbg/libcdbutils.a
    cp third-party/cdb/rel-32-libcdbutils.a lib/libcdbutils.a
fi


