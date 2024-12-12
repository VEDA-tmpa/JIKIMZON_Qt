#!/bin/bash

cd build
make clean
qmake ../test.pro
make -j2
