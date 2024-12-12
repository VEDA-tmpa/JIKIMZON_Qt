cd build
qmake -o Makefile ..\JIKIMZON.pro
mingw32-make clean
mingw32-make debug -j4
cd ..
./build/debug/JIKIMZON.exe