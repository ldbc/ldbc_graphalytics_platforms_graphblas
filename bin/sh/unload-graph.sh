#!/bin/bash

set -eo pipefail

rootdir="$( cd "$( dirname "${BASH_SOURCE[0]:-${(%):-%x}}" )" >/dev/null 2>&1 && pwd )/../.."

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
