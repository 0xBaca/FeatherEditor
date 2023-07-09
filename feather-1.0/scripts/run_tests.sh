#!/bin/sh

BASEDIR=$(dirname $0)

export PATH=$PATH:/Applications/CMake.app/Contents/bin
cd $BASEDIR/.. && make clean && make TEST='true' all_tests
