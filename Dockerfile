FROM ubuntu:20.04

# to prevent tzdata from requiring user input
# https://askubuntu.com/a/1013396/415610
ENV DEBIAN_FRONTEND=noninteractive

RUN apt update
RUN apt install -y git gcc g++ cmake curl unzip
RUN apt install -y openjdk-8-jdk maven
RUN apt install -y libsuitesparse-dev

WORKDIR /opt

# Download LAGraph
RUN git clone https://github.com/GraphBLAS/LAGraph

# Download Graphalytics
RUN git clone https://github.com/ldbc/ldbc_graphalytics

# Download example graphs
WORKDIR /opt/graphs
RUN curl 'https://atlarge.ewi.tudelft.nl/graphalytics/zip/example-directed.zip'   > example-directed.zip   && unzip example-directed.zip
RUN curl 'https://atlarge.ewi.tudelft.nl/graphalytics/zip/example-undirected.zip' > example-undirected.zip && unzip example-undirected.zip

WORKDIR /opt/LAGraph
RUN cmake .
RUN JOBS=$(nproc) make install

# Intall Graphalytics
WORKDIR /opt/ldbc_graphalytics/
RUN mvn install

# Copy the project
WORKDIR /opt/
COPY . /opt/graphalytics-graphblas
WORKDIR /opt/graphalytics-graphblas/

# Build the project
RUN export CPATH=/opt/GraphBLAS/Include/ && ./init.sh

# Run benchmark
WORKDIR /opt/graphalytics-graphblas/graphalytics-1.6.0-graphblas-0.1-SNAPSHOT
RUN bin/sh/run-benchmark.sh
