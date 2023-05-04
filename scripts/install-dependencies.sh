#!/bin/bash

set -eo pipefail

rootdir="$( cd "$( dirname "${BASH_SOURCE[0]:-${(%):-%x}}" )" >/dev/null 2>&1 && pwd )/.."

cd ${rootdir}

scripts/install-prerequisites.sh
scripts/install-graphblas.sh
scripts/install-lagraph.sh
