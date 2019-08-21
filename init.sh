#/bin/sh

set -e

GRAPHS_DIR=${1:-~/graphs}
PROJECT=graphalytics-1.0.0-graphblas-0.1-SNAPSHOT

rm -rf $PROJECT
mvn package
tar xf $PROJECT-bin.tar.gz
cd $PROJECT/
cp -r config-template config
sed -i "s|^graphs.root-directory =$|graphs.root-directory = $GRAPHS_DIR|g" config/benchmark.properties
sed -i "s|^graphs.validation-directory =$|graphs.validation-directory = $GRAPHS_DIR|g" config/benchmark.properties
bin/sh/compile-benchmark.sh
