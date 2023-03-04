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

# convert .v/.e files to matrix market format (.mtx) with the vertex ID mapping (.vtx)

if [[ ! -f ${OUTPUT_PATH}/graph.mtx && ! -f ${OUTPUT_PATH}/graph.vtx ]]; then
    bin/sh/relabel.py \
        --graph-name ${GRAPH_NAME} \
        --input-vertex ${INPUT_VERTEX_PATH} \
        --input-edge ${INPUT_EDGE_PATH} \
        --output-path ${OUTPUT_PATH} \
        --weighted ${WEIGHTED} \
        --directed ${DIRECTED}
else
    echo "Transformed mtx/vtx files already exist, no conversion required"
fi

# convert .mtx/.vtx files to binary formats
# if [[ ! -f ${OUTPUT_PATH}/graph.grb && ! -f ${OUTPUT_PATH}/graph.vtb ]]; then
#     bin/exe/converter \
#         --binary true \
#         --input-vertex ${INPUT_VERTEX_PATH} \
#         --input-edge ${INPUT_EDGE_PATH} \
#         --output-path ${OUTPUT_PATH} \
#         --weighted ${WEIGHTED} \
#         --directed ${DIRECTED}
# else
#     echo "Transformed grb/vtb files already exist, no conversion required"
# fi
