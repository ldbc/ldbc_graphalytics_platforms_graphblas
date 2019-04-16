# Graphalytics GraphBLAS platform driver

GraphBLAS... (TODO: provide a short description on GraphBLAS). To execute Graphalytics benchmark on GraphBLAS, follow the steps in the Graphalytics tutorial on [Running Benchmark](https://github.com/ldbc/ldbc_graphalytics/wiki/Manual%3A-Running-Benchmark) with the GraphBLAS-specific instructions listed below.

### Prerequisites
Get [SuiteSparse:GraphBLAS](https://github.com/sergiud/SuiteSparse/tree/master/GraphBLAS) and install with `sudo make install`.
We tested the code with GraphBLAS 2.2.2 included in SuiteSparse 5.4.0.

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

