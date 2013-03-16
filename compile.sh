#!/bin/sh
make distclean;
git pull;
make -j5;
exit 0
