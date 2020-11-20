#!/bin/bash

set -e

# Ensure the configuration file exists
if [ "$(uname)" == "Darwin" ]; then
  rootdir=$(dirname $(greadlink -f ${BASH_SOURCE[0]}))/../..
else
  rootdir=$(dirname $(readlink -f ${BASH_SOURCE[0]}))/../..
fi
config="${rootdir}/config"
if [ ! -f "$config/platform.properties" ]; then
	echo "Missing mandatory configuration file: $config/platform.properties" >&2
	exit 1
fi

# TODO Build binaries
mkdir -p $rootdir/bin/exe
(cd $rootdir/bin/exe && cmake -DCMAKE_BUILD_TYPE=Release ../../src/main/c && JOBS=$(nproc) make all VERBOSE=1)

if [ $? -ne 0 ]
then
    echo "Compilation failed"
    exit 1
fi
