#!/bin/sh

export PATH=$PATH:/Applications/CMake.app/Contents/bin
cd .. && make clean && make TEST='true' all_tests
