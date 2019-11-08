# Using OpenJDK 8 to run the LDBC Graphalytics benchmark harness
FROM openjdk:8-jdk-stretch

# Prerequisites
RUN apt-get update
RUN apt-get install -y bash curl maven cmake m4 g++

# Download GraphBLAS
WORKDIR /opt
RUN curl -L 'http://faculty.cse.tamu.edu/davis/GraphBLAS/GraphBLAS-3.1.1.tar.gz' | tar -xz
# drop version number from the GraphBLAS directory
RUN mv GraphBLAS-* GraphBLAS

# Download LAGraph
RUN git clone https://github.com/GraphBLAS/LAGraph

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
RUN make
RUN make install
RUN ldconfig

# Build Graphalytics
WORKDIR /opt/ldbc_graphalytics
RUN mvn install

# Copy the project
WORKDIR /opt/
COPY . /opt/graphalytics-graphblas
WORKDIR /opt/graphalytics-graphblas/

# Build the project
RUN mvn package
RUN tar xf *.tar.gz
RUN mv graphalytics-*/ graphalytics
WORKDIR /opt/graphalytics-graphblas/graphalytics

# Run benchmark
RUN cp -r config-template config
RUN bin/sh/run-benchmark.sh
