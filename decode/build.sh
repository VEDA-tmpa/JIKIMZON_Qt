#!/bin/bash
cd ./build
make clean
qmake ../JIKIMZON_Qt.pro
make -j2
cd ..