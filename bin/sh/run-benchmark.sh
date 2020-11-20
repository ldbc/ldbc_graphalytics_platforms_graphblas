#!/bin/bash

set -e

if [ "$(uname)" == "Darwin" ]; then
  rootdir=$(dirname $(greadlink -f ${BASH_SOURCE[0]}))/../..
else
  rootdir=$(dirname $(readlink -f ${BASH_SOURCE[0]}))/../..
fi
config="${rootdir}/config/"

function print-usage() {
	echo "Usage: ${BASH_SOURCE[0]} [--config <dir>]" >&2
}

# Parse the command-line arguments
while :
do
	case "$1" in
		--config)                      # Use a different config directory
			if [ "$(uname)" == "Darwin" ]; then
				config="$(greadlink -f "$2")"
			else
				config="$(readlink -f "$2")"
			fi
			echo "Using config: $config"
			shift 2
			;;
		--)                            # End of options
			shift
			break
			;;
		-*)                            # Unknown command line option
			echo "Unknown option: $1" >&2
			print-usage
			exit 1
			;;
		*)                             # End of options
			break
			;;
	esac
done

# Execute platform specific initialization
export config=$config
. ${rootdir}/bin/sh/prepare-benchmark.sh "$@"

# Verify that the library jar is set
if [ "$LIBRARY_JAR" = "" ]; then
	echo "The prepare-benchmark.sh script must set variable \$LIBRARY_JAR" >&2
	exit 1
fi

# Run the benchmark
export CLASSPATH=$config:$(find ${rootdir}/$LIBRARY_JAR):$platform_classpath
echo $CLASSPATH

java -cp $CLASSPATH $java_opts science.atlarge.graphalytics.BenchmarkSuite
