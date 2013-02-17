#!/bin/bash

LINCORE_HOME=INSTALL_DIR

export LD_LIBRARY_PATH=$LINCORE_HOME/lib:$LD_LIBRARY_PATH

cd $LINCORE_HOME/var
touch lincore.pid
$LINCORE_HOME/bin/lincored --config=$LINCORE_HOME/config/lincore.cfg &

#  2>lincore.log

