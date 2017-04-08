#!/bin/sh

TANAME=tdsx
# stop eventually running ${TANAME}
if [ -x /opt/ext/tac/${TANAME}/start.sh ]; then
 /opt/ext/tac/${TANAME}/start.sh stop
fi
exit 0
