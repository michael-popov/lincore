#/bin/bash

LINCORE_HOME=INSTALL_DIR
LINCORE_PID=`cat $LINCORE_HOME/var/lincore.pid`

COUNT=`ps -ef | grep lincore | grep $LINCORE_PID | wc -l`
if [ $COUNT -ne 0 ]; then
    kill $LINCORE_PID
    while [ -e /proc/$LINCORE_PID ]; do sleep 0.1; done
fi

