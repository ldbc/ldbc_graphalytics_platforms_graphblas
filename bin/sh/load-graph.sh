#!/bin/bash

set -e

if [ "$(uname)" == "Darwin" ]; then
  rootdir=$(dirname $(greadlink -f ${BASH_SOURCE[0]}))/../..
else
  rootdir=$(dirname $(readlink -f ${BASH_SOURCE[0]}))/../..
fi

# Parse commandline instructions (provided by Graphalytics).
while [[ $# -gt 1 ]] # Parse two arguments: [--key value] or [-k value]
  do
  key="$1"
  value="$2"

  case ${key} in

    --graph-name)
      # not used
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

if [[ ! -f ${OUTPUT_PATH}/graph.grb && ! -f ${OUTPUT_PATH}/graph.vtb ]]; then
    bin/exe/converter \
        --binary true \
        --input-vertex ${INPUT_VERTEX_PATH} \
        --input-edge ${INPUT_EDGE_PATH} \
        --output-matrix  ${OUTPUT_PATH}/graph.grb \
        --output-mapping ${OUTPUT_PATH}/graph.vtb \
        --weighted ${WEIGHTED} \
        --directed ${DIRECTED}
else
    echo "Transformed file already existing, no load required"
fi
