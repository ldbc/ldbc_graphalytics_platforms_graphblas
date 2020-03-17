# Using OpenJDK 8 to run the LDBC Graphalytics benchmark harness
FROM openjdk:8-jdk-stretch

# Prerequisites
RUN apt-get update
RUN apt-get install -y bash curl maven cmake m4 g++

# Download GraphBLAS
WORKDIR /opt
RUN git clone --depth 1 --branch v3.2.0 https://github.com/DrTimothyAldenDavis/GraphBLAS

# Download LAGraph
RUN git clone --depth 1 https://github.com/GraphBLAS/LAGraph

# Download Graphalytics
RUN git clone https://github.com/ldbc/ldbc_graphalytics

# Download example graphs
WORKDIR /opt/graphs
RUN curl 'https://atlarge.ewi.tudelft.nl/graphalytics/zip/example-directed.zip'   > example-directed.zip   && unzip example-directed.zip
RUN curl 'https://atlarge.ewi.tudelft.nl/graphalytics/zip/example-undirected.zip' > example-undirected.zip && unzip example-undirected.zip

# Build GraphBLAS
WORKDIR /opt/GraphBLAS
RUN JOBS=$(nproc) make
RUN make install
RUN ldconfig

# Build LAGraph
WORKDIR /opt/LAGraph
RUN JOBS=$(nproc) make
RUN make install
RUN ldconfig

# Copy the project
WORKDIR /opt/
COPY . /opt/graphalytics-graphblas
WORKDIR /opt/graphalytics-graphblas/

# Build the project
./init.sh

# Run benchmark
RUN bin/sh/run-benchmark.sh
