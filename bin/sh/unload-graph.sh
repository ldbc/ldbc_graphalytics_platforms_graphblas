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

  case $key in

    --graph-name)
      GRAPH_NAME="$value"
      shift;;

    --output-path)
      OUTPUT_PATH="$value"
      shift;;

    *)
      echo "Error: invalid option: " "$key"
      exit 1
      ;;
  esac
  shift
done

# TODO Reconstruct executable commandline instructions (platform-specific).
if [[ ! -z "${GRAPH_NAME}" && "${OUTPUT_PATH}" == *"${GRAPH_NAME}"* ]];then
    COMMAND=""
    #COMMAND="rm -r $OUTPUT_PATH"
else
    echo "Failed to delete graph ${GRAPH_NAME}, path ${OUTPUT_PATH} does not contain graph name (unsafe)."
    exit 1
fi

echo "Executing graph unloader:" ["$COMMAND"]
$COMMAND
