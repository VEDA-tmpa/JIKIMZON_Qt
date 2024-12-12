#!/bin/bash

cd build
qmake ../JIKIMZON.pro
make clean
make -j2
