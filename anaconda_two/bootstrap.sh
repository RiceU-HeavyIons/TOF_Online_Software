#!/bin/sh

# example:
# ./bootstrap.sh CONFIG += static
# ./bootstrap.sh CONFIG += shared PCAN=fake
for d in fakepcan src app .
do
  pushd $d
  qmake $@
  popd
done
