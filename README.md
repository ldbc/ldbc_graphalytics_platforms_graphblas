# SuiteSparse:GraphBLAS driver for LDBC Graphalytics

[![Build Status](https://travis-ci.org/ftsrg/ldbc-graphalytics-platform-graphblas.svg?branch=master)](https://travis-ci.org/ftsrg/ldbc-graphalytics-platform-graphblas)

Platform driver for the [LDBC Graphalytics benchmark](https://graphalytics.org) using the building blocks of the [GraphBLAS standard](http://graphblas.org/) and its [SuiteSparse:GraphBLAS reference implementation](http://faculty.cse.tamu.edu/davis/GraphBLAS.html).

To execute the Graphalytics benchmark on GraphBLAS, follow the steps in the Graphalytics tutorial on [Running Benchmark](https://github.com/ldbc/ldbc_graphalytics/wiki/Manual%3A-Running-Benchmark) with the GraphBLAS-specific instructions listed below.

### Prerequisites

Install CMake (the package in Ubuntu 18.04 satisfies the minimum version requirements):

```bash
sudo apt install cmake
```

Set up Make to run in parallel:

```bash
export JOBS=$(nproc)
```

The default compiler on most Linux system is the GCC. To use the Intel C++ Compiler, get a license, install it, [configure it](https://github.com/FTSRG/cheat-sheets/wiki/GraphBLAS), and issue the following commands:

```bash
export CC=icc
export CXX=icc
```

Get [SuiteSparse:GraphBLAS v3.2.0+](http://faculty.cse.tamu.edu/davis/GraphBLAS.html), decompress it and install:

```bash
git clone --depth 1 --branch v3.2.0 --single-branch https://github.com/DrTimothyAldenDavis/GraphBLAS
cd GraphBLAS
make && sudo make install && sudo ldconfig
cd ..
```

Get [LAGraph](https://github.com/GraphBLAS/LAGraph), and install it:

```bash
git clone https://github.com/GraphBLAS/LAGraph
cd LAGraph
make && sudo make install && sudo ldconfig
cd ..
```

### Configure the benchmark

Follow the steps given in the [LDBC Graphalytics wiki](https://github.com/ldbc/ldbc_graphalytics/wiki).

To initialize the benchmark package, run:

```bash
./init.sh MY_GRAPH_DIR [MY_MATRICES_DIR]
```

where
* `MY_GRAPH_DIR` is the directory of the graphs and the validation data. The default value is `~/graphs`.
* `MY_MATRICES_DIR` (optional) is the directory of the pre-generated matrix files (in Matrix Market or binary format). The default value is `~/matrices`.

Edit the rest of the configurations (e.g. graphs to be included in the benchmark) in the `config` directory. Also, inspect `config/platform.properties` and check the value of `platform.graphblas.num-threads`.

Run the benchmark with:

```bash
bin/sh/run-benchmark.sh
```

### Docker

There's a `Dockerfile` included which shows how to compile GraphBLAS, LAGraph and how to use this driver.

To build it, run:

```bash
docker build . --tag ldbc/graphalytics-graphblas
```

Depending on the number of CPU cores and the speed of your internet connection, building the image takes approx. 3-30 minutes.
