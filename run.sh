#/bin/sh

set -e

PROJECT=graphalytics-1.0.0-graphblas-0.1-SNAPSHOT

rm -rf $PROJECT
mvn package
tar xf $PROJECT-bin.tar.gz
cd $PROJECT/
cp -r config-template config
bin/sh/compile-benchmark.sh
bin/sh/run-benchmark.sh
