#!/bin/bash

set -eo pipefail

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

mkdir -p $rootdir/bin/exe
cd $rootdir/bin/exe
cmake -DCMAKE_BUILD_TYPE=Release ../../src/main/c
JOBS=$(nproc) make all VERBOSE=1
