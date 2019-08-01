# SuiteSparse:GraphBLAS driver for LDBC Graphalytics

[![Build Status](https://travis-ci.com/FTSRG/ldbc-graphalytics-platform-graphblas.svg?branch=master)](https://travis-ci.com/FTSRG/ldbc-graphalytics-platform-graphblas)

Platform driver for the [LDBC Graphalytics benchmark](https://graphalytics.org) using the building blocks of the [GraphBLAS standard](http://graphblas.org/) and its [SuiteSparse:GraphBLAS reference implementation](http://faculty.cse.tamu.edu/davis/GraphBLAS.html).

To execute Graphalytics benchmark on GraphBLAS, follow the steps in the Graphalytics tutorial on [Running Benchmark](https://github.com/ldbc/ldbc_graphalytics/wiki/Manual%3A-Running-Benchmark) with the GraphBLAS-specific instructions listed below.

### Prerequisites

Get [SuiteSparse:GraphBLAS v3.0.1+](http://faculty.cse.tamu.edu/davis/GraphBLAS.html), decompress it and install:

```bash
wget http://faculty.cse.tamu.edu/davis/GraphBLAS/GraphBLAS-3.0.1.tar.gz
tar xf GraphBLAS-3.0.1.tar.gz
cd GraphBLAS
make && sudo make install && sudo ldconfig
```

Get [LAGraph](https://github.com/GraphBLAS/LAGraph), and install it:

```bash
git clone https://github.com/GraphBLAS/LAGraph
cd LAGraph
make && sudo make install && sudo ldconfig
```

### Configure the benchmark

Follow the steps given in the [LDBC Graphalytics wiki](https://github.com/ldbc/ldbc_graphalytics/wiki).

A snippets to get things started:

```bash
mvn package
tar xf graphalytics-*.tar.gz
cd graphalytics-*
cp -r config-template config
bin/sh/compile-benchmark.sh
```

Set the benchmark configurations, including the graphs to be used, the directories of the graphs and the validation data in `config/benchmark.properties`, `config/benchmarks/...`, etc. Then, run the benchmark with:

```bash
bin/sh/run-benchmark.sh
```

### Docker

There's a `Dockerfile` included which shows how to compile GraphBLAS, LAGraph and how to use this driver.

To build it, run:

```bash
docker build . --tag ldbc/graphalytics-graphblas
```

The compilation takes approx. 15-30 minutes.
