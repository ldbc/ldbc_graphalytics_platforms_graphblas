#/bin/sh

set -e

GRAPHS_DIR=${1:-~/graphs}
GRAPHALYTICS_VERSION=1.3.0-SNAPSHOT
PROJECT_VERSION=0.1-SNAPSHOT
PROJECT=graphalytics-$GRAPHALYTICS_VERSION-graphblas-$PROJECT_VERSION

rm -rf $PROJECT
mvn package
tar xf $PROJECT-bin.tar.gz
cd $PROJECT/
cp -r config-template config
sed -i "s|^graphs.root-directory =$|graphs.root-directory = $GRAPHS_DIR|g" config/benchmark.properties
sed -i "s|^graphs.validation-directory =$|graphs.validation-directory = $GRAPHS_DIR|g" config/benchmark.properties
bin/sh/compile-benchmark.sh
