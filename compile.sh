#!/bin/sh
make distclean;
git pull;
qmake;
make -j5;
exit 0
