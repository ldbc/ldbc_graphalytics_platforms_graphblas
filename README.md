# SuiteSparse:GraphBLAS driver for LDBC Graphalytics

[![Build Status](https://circleci.com/gh/ldbc/ldbc_graphalytics_platforms_graphblas.svg?style=svg)](https://app.circleci.com/pipelines/github/ldbc/ldbc_graphalytics_platforms_graphblas)

Platform driver for the [LDBC Graphalytics benchmark](https://graphalytics.org) using [SuiteSparse:GraphBLAS](https://github.com/DrTimothyAldenDavis/GraphBLAS) and [LAGraph](https://github.com/GraphBLAS/LAGraph).

To execute the Graphalytics benchmark on GraphBLAS, follow the steps in the Graphalytics tutorial on [Running Benchmark](https://github.com/ldbc/ldbc_graphalytics/wiki/Manual%3A-Running-Benchmark) with the GraphBLAS-specific instructions listed below.

### Prerequisites

Make sure you have the following software packages installed:

* CMake
* GCC, Clang, or ICC

### Dependencies

1. Configure Make to run in parallel:

    ```bash
    export JOBS=$(nproc)
    ```

1. Get [SuiteSparse:GraphBLAS v7.4.2+](https://github.com/DrTimothyAldenDavis/GraphBLAS), decompress it and install:

    ```bash
    git clone --depth 1 --branch v7.4.2 --single-branch https://github.com/DrTimothyAldenDavis/GraphBLAS
    cd GraphBLAS
    make && sudo make install && sudo ldconfig
    cd ..
    ```

1. Get [LAGraph v1.0.1+](https://github.com/GraphBLAS/LAGraph), and install it:

    ```bash
    git clone --depth 1 --branch v1.0.1 --single-branch https://github.com/GraphBLAS/LAGraph
    cd LAGraph
    make && sudo make install && sudo ldconfig
    cd ..
    ```

### Running the benchmark

1. To initialize the benchmark package, run:

    ```bash
    ./init.sh ${GRAPHS_DIR} ${MATRICES_DIR}
    ```

    where
    * `GRAPHS_DIR` is the directory of the graphs and the validation data. The argument is optional and its default value is `~/graphs`.
    * `MATRICES_DIR` is the directory of the pre-generated matrix files (in Matrix Market format). The argument is optional and its default value is `~/matrices`.

    Edit the rest of the configurations (e.g. graphs to be included in the benchmark) in the `config` directory. Also, inspect `config/platform.properties` and check whether the value of `platform.graphblas.num-threads` was set correctly.

1. Navigate to the directory created by the `init.sh` script:

    ```bash
    cd graphalytics-*-graphblas-*/
    ```

1. Run the benchmark with the following command:

    ```bash
    bin/sh/run-benchmark.sh
    ```

### Intel C++ Compiler

To use the Intel C++ Compiler, get a license, install it, [configure it](https://github.com/FTSRG/cheat-sheets/wiki/GraphBLAS), and issue the following commands:

```bash
export CC=icc
export CXX=icc
```
