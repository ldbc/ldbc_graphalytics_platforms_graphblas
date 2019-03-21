# Graphalytics Ejml platform driver

Ejml... (TODO: provide a short description on Ejml). To execute Graphalytics benchmark on Ejml, follow the steps in the Graphalytics tutorial on [Running Benchmark](https://github.com/ldbc/ldbc_graphalytics/wiki/Manual%3A-Running-Benchmark) with the Ejml-specific instructions listed below.

### Obtain the platform driver
There are two possible ways to obtain the Ejml platform driver:

 1. **Download the (prebuilt) [Ejml platform driver](http://graphalytics.site/dist/stable/) distribution from our website.

 2. **Build the platform drivers**: 
  - Download the source code from this repository.
  - Execute `mvn clean package` in the root directory (See details in [Software Build](https://github.com/ldbc/ldbc_graphalytics/wiki/Documentation:-Software-Build)).
  - Extract the distribution from  `graphalytics-{graphalytics-version}-ejml-{platform-version}.tar.gz`.

### Verify the necessary prerequisites
The softwares listed below are required by the Ejml platform driver, which should be properly configured in the cluster environment....

### Adjust the benchmark configurations
Adjust the Ejml configurations in `config/platform.properties`...

