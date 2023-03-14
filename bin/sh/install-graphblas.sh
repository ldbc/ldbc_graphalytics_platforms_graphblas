#!/bin/bash

set -eo pipefail

rootdir="$( cd "$( dirname "${BASH_SOURCE[0]:-${(%):-%x}}" )" >/dev/null 2>&1 && pwd )/../.."

rm -rf GraphBLAS
git clone --depth 1 --branch v7.4.3 --single-branch https://github.com/DrTimothyAldenDavis/GraphBLAS
cd GraphBLAS
cd build

# The optional "--compact" argument triggers the "compact" build in SuiteSparse:GraphBLAS
# which omits compiling the code generated for each individual semiring.
# This significantly speeds up the build but make the execution slower, therefore, it should
# be avoided for benchmarking.
if [ $# -gt 0 ] && [ "$1" == "--compact" ]; then
    echo 'compact build'
    cmake .. -DCMAKE_C_FLAGS="-DGBCUDA_DEV=1"
else
    cmake ..
fi

make -j$(nproc)
sudo make install

if [ "$(uname)" != "Darwin" ]; then
    sudo ldconfig
fi

cd ../..
