cd build
qmake -o Makefile ..\JIKIMZON_Qt.pro
mingw32-make clean
mingw32-make debug -j3
cd ..
