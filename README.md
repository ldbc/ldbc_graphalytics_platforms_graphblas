# SuiteSparse:GraphBLAS driver for LDBC Graphalytics

[![Build Status](https://travis-ci.com/FTSRG/ldbc-graphalytics-platform-graphblas.svg?branch=master)](https://travis-ci.com/FTSRG/ldbc-graphalytics-platform-graphblas)

GraphBLAS... (TODO: provide a short description on GraphBLAS). To execute Graphalytics benchmark on GraphBLAS, follow the steps in the Graphalytics tutorial on [Running Benchmark](https://github.com/ldbc/ldbc_graphalytics/wiki/Manual%3A-Running-Benchmark) with the GraphBLAS-specific instructions listed below.

### Prerequisites
Get SuiteSparse:GraphBLAS version 2.3.2+ as a [standalone package](http://faculty.cse.tamu.edu/davis/GraphBLAS.html), decompress it and install:

```console
sudo make install && sudo ldconfig
```

The whole SuiteSparse suite follows a slower release cycle, but is available at [the authors' site](http://faculty.cse.tamu.edu/davis/suitesparse.html), with
[multiple](https://github.com/sergiud/SuiteSparse/tree/master/GraphBLAS)
[mirrors](https://github.com/jluttine/suitesparse)
on GitHub.

### Obtain the platform driver
There are two possible ways to obtain the GraphBLAS platform driver:

 1. **Download the (prebuilt) [GraphBLAS platform driver](http://graphalytics.site/dist/stable/) distribution from our website.

 2. **Build the platform drivers**: 
  - Download the source code from this repository.
  - Execute `mvn clean package` in the root directory (See details in [Software Build](https://github.com/ldbc/ldbc_graphalytics/wiki/Documentation:-Software-Build)).
  - Extract the distribution from  `graphalytics-{graphalytics-version}-graphblas-{platform-version}.tar.gz`.

### Verify the necessary prerequisites
The softwares listed below are required by the GraphBLAS platform driver, which should be properly configured in the cluster environment....

### Adjust the benchmark configurations
Adjust the GraphBLAS configurations in `config/platform.properties`...

