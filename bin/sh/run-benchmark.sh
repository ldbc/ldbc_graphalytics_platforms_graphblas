#!/bin/bash

set -eo pipefail

rootdir="$( cd "$( dirname "${BASH_SOURCE[0]:-${(%):-%x}}" )" >/dev/null 2>&1 && pwd )/../.."
config="${rootdir}/config/"

function print-usage() {
	echo "Usage: ${BASH_SOURCE[0]} [--config <dir>]" >&2
}

# Parse the command-line arguments
while :
do
	case "$1" in
		--config)                      # Use a different config directory
			config=$2
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
	echo "The prepare-benchmark.sh script must set variable \${LIBRARY_JAR}" >&2
	exit 1
fi

# Run the benchmark
export CLASSPATH=$config:$(find ${rootdir}/$LIBRARY_JAR):$platform_classpath
echo $CLASSPATH

java -cp $CLASSPATH $java_opts science.atlarge.graphalytics.BenchmarkSuite
