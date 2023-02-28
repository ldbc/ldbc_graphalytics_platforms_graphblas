git clean -xdf .
export rootdir=`pwd`
mkdir -p $rootdir/bin/exe
cd $rootdir/bin/exe
cmake -DCMAKE_BUILD_TYPE=Release ../../src/main/c
JOBS=$(nproc) make all VERBOSE=1
