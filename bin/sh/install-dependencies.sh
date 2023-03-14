#!/bin/bash

set -eo pipefail

rootdir="$( cd "$( dirname "${BASH_SOURCE[0]:-${(%):-%x}}" )" >/dev/null 2>&1 && pwd )/../.."

cd ${rootdir}
bin/sh/install-prerequisites.sh
bin/sh/install-graphblas.sh
bin/sh/install-lagraph.sh
