#!/bin/bash

set -eo pipefail

rootdir="$( cd "$( dirname "${BASH_SOURCE[0]:-${(%):-%x}}" )" >/dev/null 2>&1 && pwd )/../.."

rm -rf LAGraph
git clone --depth 1 --branch dev --single-branch https://github.com/GraphBLAS/LAGraph
cd LAGraph
make -j$(nproc)
sudo make install

if [ "$(uname)" != "Darwin" ]; then
    sudo ldconfig
fi

cd ..
