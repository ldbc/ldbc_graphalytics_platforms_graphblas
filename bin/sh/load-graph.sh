#!/bin/bash

set -eo pipefail

rootdir="$( cd "$( dirname "${BASH_SOURCE[0]:-${(%):-%x}}" )" >/dev/null 2>&1 && pwd )/../.."

# Parse commandline instructions (provided by Graphalytics).
while [[ $# -gt 1 ]] # Parse two arguments: [--key value] or [-k value]
  do
  key="$1"
  value="$2"

  case ${key} in

    --graph-name)
      GRAPH_NAME="$value"
      shift;;

    --input-vertex-path)
      INPUT_VERTEX_PATH="$value"
      shift;;

    --input-edge-path)
      INPUT_EDGE_PATH="$value"
      shift;;

    --output-path)
      OUTPUT_PATH="$value"
      shift;;

    --directed)
      DIRECTED="$value"
      shift;;

    --weighted)
      WEIGHTED="$value"
      shift;;

    *)
      echo "Error: invalid option: " "$key"
      exit 1
      ;;
  esac
  shift
done

mkdir -p ${OUTPUT_PATH}

# convert .v/.e files to .vtx/.mtx Matrix Market files
if [[ ! -f ${OUTPUT_PATH}/graph.vtx && ! -f ${OUTPUT_PATH}/graph.mtx ]]; then
    bin/py/relabel.py \
        --graph-name ${GRAPH_NAME} \
        --input-vertex ${INPUT_VERTEX_PATH} \
        --input-edge ${INPUT_EDGE_PATH} \
        --output-path ${OUTPUT_PATH} \
        --weighted ${WEIGHTED} \
        --directed ${DIRECTED}
else
    echo "Transformed Matrix Market (mtx/vtx) files already exist, no relabelling required"
fi

# convert .mtx/.vtx files to binary .vtb/.grb files
if [[ ! -f ${OUTPUT_PATH}/graph.vtb && ! -f ${OUTPUT_PATH}/graph.grb ]]; then
    bin/exe/converter --data-dir ${OUTPUT_PATH}
else
    echo "Transformed binary files (grb/vtb) already exist, no conversion required"
fi
