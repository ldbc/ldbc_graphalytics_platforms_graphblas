# SuiteSparse:GraphBLAS driver for LDBC Graphalytics

[![Build Status](https://travis-ci.com/FTSRG/ldbc-graphalytics-platform-graphblas.svg?branch=master)](https://travis-ci.com/FTSRG/ldbc-graphalytics-platform-graphblas)

GraphBLAS... (TODO: provide a short description on GraphBLAS). To execute Graphalytics benchmark on GraphBLAS, follow the steps in the Graphalytics tutorial on [Running Benchmark](https://github.com/ldbc/ldbc_graphalytics/wiki/Manual%3A-Running-Benchmark) with the GraphBLAS-specific instructions listed below.

### Prerequisites

Get [SuiteSparse:GraphBLAS v3.0.1+](http://faculty.cse.tamu.edu/davis/GraphBLAS.html), decompress it and install:

```console
wget http://faculty.cse.tamu.edu/davis/GraphBLAS/GraphBLAS-3.0.1.tar.gz
tar xf GraphBLAS-3.0.1.tar.gz
cd GraphBLAS
make && sudo make install && sudo ldconfig
```

Get [LAGraph](https://github.com/GraphBLAS/LAGraph), and install it

```console
git clone https://github.com/GraphBLAS/LAGraph
cd LAGraph
make && sudo make install && sudo ldconfig
```

### Configure the benchmark

Follow the steps given in the [LDBC Graphalytics wiki](https://github.com/ldbc/ldbc_graphalytics/wiki).

A snippets to get things started:

```console
mvn package
tar xf graphalytics-*.tar.gz
cd graphalytics-*
cp -r config-template config
bin/sh/compile-benchmark.sh
```

Set the benchmark configurations, including the graphs to be used, the directories of the graphs and the validation data in `config/benchmark.properties`, `config/benchmarks/...`, etc. Then, run the benchmark with:

```console
bin/sh/run-benchmark.sh
```

### Docker

There's a `Dockerfile` included which shows how to compile GraphBLAS, LAGraph and how to use this driver.
