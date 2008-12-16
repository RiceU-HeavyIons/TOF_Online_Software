#!/bin/sh

if [ -f Makefile ]; then
  make distclean
fi

for d in fakepcan lib
do
  pushd $d
  rm -rf *.dylib *.a *.so
  popd
done

