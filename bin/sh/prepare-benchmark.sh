#!/bin/bash

# Ensure the configuration file exists
if [ ! -f "$config/platform.properties" ]; then
	echo "Missing mandatory configuration file: $config/platform.properties" >&2
	exit 1
fi

# Set library jar
export LIBRARY_JAR=`ls lib/graphalytics-*default*.jar`
