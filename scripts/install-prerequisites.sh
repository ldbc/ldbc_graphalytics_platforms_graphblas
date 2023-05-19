#!/bin/bash

set -eo pipefail

cd "$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
cd ..

if [[ ! -z $(which yum) ]]; then
    sudo yum install -y python3-pip cmake g++ maven
elif [[ ! -z $(which apt-get) ]]; then
    sudo apt-get update
    sudo apt-get install -y python3-pip cmake g++ maven
elif [ "$(uname)" == "Darwin" ]; then
    brew install cmake maven coreutils
fi

pip3 install --user duckdb==0.8.1
