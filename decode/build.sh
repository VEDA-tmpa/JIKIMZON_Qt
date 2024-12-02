#!/bin/bash
cd ./build
qmake ../JIKIMZON.pro
make -j3
cd ..