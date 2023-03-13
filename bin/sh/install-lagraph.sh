#!/bin/bash

set -eo pipefail

if [ "$(uname)" == "Darwin" ]; then
  rootdir=$(dirname $(greadlink -f ${BASH_SOURCE[0]}))/../..
else
  rootdir=$(dirname $(readlink -f ${BASH_SOURCE[0]}))/../..
fi

rm -rf LAGraph
git clone --depth 1 --branch dev --single-branch https://github.com/GraphBLAS/LAGraph
cd LAGraph
make -j$(nproc)
sudo make install

if [ "$(uname)" != "Darwin" ]; then
    sudo ldconfig
fi

cd ..
