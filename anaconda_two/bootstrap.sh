#!/bin/sh

for d in fakepcan src app .
do
  pushd $d
  qmake
  popd
done
