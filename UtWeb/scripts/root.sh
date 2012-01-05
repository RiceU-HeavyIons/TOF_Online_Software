#!/bin/sh
export ROOTSYS=/usr/local/root
export LD_LIBRARY_PATH=${ROOTSYS}/lib
LOGF=/dev/null
${ROOTSYS}/bin/root -b -q $1 >& ${LOGF}
