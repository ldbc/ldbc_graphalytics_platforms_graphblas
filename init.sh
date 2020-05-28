#/bin/sh

set -e

GRAPHS_DIR=${1:-~/graphs}
MATRICES_DIR=${2:-~/matrices}
GRAPHALYTICS_VERSION=1.4.0-SNAPSHOT
PROJECT_VERSION=0.1-SNAPSHOT
PROJECT=graphalytics-$GRAPHALYTICS_VERSION-graphblas-$PROJECT_VERSION

rm -rf $PROJECT
mvn package
tar xf $PROJECT-bin.tar.gz
cd $PROJECT/

cp -r config-template config
# set directories
sed -i "s|^graphs.root-directory =$|graphs.root-directory = $GRAPHS_DIR|g" config/benchmark.properties
sed -i "s|^graphs.validation-directory =$|graphs.validation-directory = $GRAPHS_DIR|g" config/benchmark.properties
# set the number of threads to use
sed -i "s|^platform.graphblas.num-threads =$|platform.graphblas.num-threads = $(nproc --all)|g" config/platform.properties

bin/sh/compile-benchmark.sh

if [ -d $MATRICES_DIR ]; then
	bin/sh/link-matrix-market-graphs.sh $MATRICES_DIR
fi
