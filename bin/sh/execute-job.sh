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

    --job-id)
      JOB_ID="$value"
      shift;;

    --log-path)
      LOG_PATH="$value"
      shift;;

    --algorithm)
      ALGORITHM="$value"
      shift;;

    --source-vertex)
      SOURCE_VERTEX="$value"
      shift;;

    --max-iteration)
      MAX_ITERATION="$value"
      shift;;

    --damping-factor)
      DAMPING_FACTOR="$value"
      shift;;

    --input-path)
      INPUT_PATH="$value"
      shift;;

    --output-path)
      OUTPUT_PATH="$value"
      shift;;

    --num-machines)
      NUM_MACHINES="$value"
      shift;;

    --num-threads)
      NUM_THREADS="$value"
      shift;;

    --directed)
      DIRECTED="$value"
      shift;;

    --num-vertices)
      NUM_VERTICES="$value"
      shift;;

    *)
      echo "Error: invalid option: " "$key"
      exit 1
      ;;
  esac
  shift
done

case $ALGORITHM in

     bfs)
       COMMAND="$rootdir/bin/exe/$ALGORITHM \
         --binary true \
         --jobid $JOB_ID \
         --dataset $INPUT_PATH \
         --output $OUTPUT_PATH \
         --directed $DIRECTED \
         --source-vertex $SOURCE_VERTEX \
         --log-path $LOG_PATH \
         --threadnum $NUM_THREADS"
       ;;

     wcc)
       COMMAND="$rootdir/bin/exe/$ALGORITHM \
         --binary true \
         --jobid $JOB_ID \
         --dataset $INPUT_PATH \
         --output $OUTPUT_PATH \
         --directed $DIRECTED \
         --log-path $LOG_PATH \
         --threadnum $NUM_THREADS"
       ;;

     pr)
       COMMAND="$rootdir/bin/exe/$ALGORITHM \
         --binary true \
         --jobid $JOB_ID \
         --dataset $INPUT_PATH \
         --output $OUTPUT_PATH \
         --directed $DIRECTED \
         --damping-factor $DAMPING_FACTOR \
         --max-iteration $MAX_ITERATION \
         --log-path $LOG_PATH \
         --threadnum $NUM_THREADS"
       ;;

     cdlp)
       COMMAND="$rootdir/bin/exe/$ALGORITHM \
         --binary true \
         --jobid $JOB_ID \
         --dataset $INPUT_PATH \
         --output $OUTPUT_PATH \
         --directed $DIRECTED \
         --max-iteration $MAX_ITERATION \
         --log-path $LOG_PATH \
         --threadnum $NUM_THREADS"
       ;;

     lcc)
       COMMAND="$rootdir/bin/exe/$ALGORITHM \
         --binary true \
         --jobid $JOB_ID \
         --dataset $INPUT_PATH \
         --output $OUTPUT_PATH \
         --directed $DIRECTED \
         --log-path $LOG_PATH \
         --threadnum $NUM_THREADS"
       ;;

     sssp)
       COMMAND="$rootdir/bin/exe/$ALGORITHM
         --binary true \
         --jobid $JOB_ID \
         --dataset $INPUT_PATH \
         --output $OUTPUT_PATH \
         --directed $DIRECTED
         --source-vertex $SOURCE_VERTEX \
         --log-path $LOG_PATH \
         --threadnum $NUM_THREADS"
       ;;

     *)
       echo "Error: algorithm $ALGORITHM not defined."
       exit 1
       ;;
esac


echo "Executing platform job" "$COMMAND"

$COMMAND & echo $! > $LOG_PATH/executable.pid
wait $!
