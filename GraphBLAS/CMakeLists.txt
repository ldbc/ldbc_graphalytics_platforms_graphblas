cmake_minimum_required(VERSION 3.13)
project(GraphBLAS)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

set(SOURCES src/utils/matrix_reader.c
        src/utils/printer.c)

add_executable(bfs_gb src/bfs.c src/bfs_source.c ${SOURCES})
add_executable(lcc_gb src/lcc.c ${SOURCES})
add_executable(lpa_gb src/lpa.c ${SOURCES})
add_executable(pagerank_gb src/pagerank.c ${SOURCES})
add_executable(sssp_gb src/sssp.c ${SOURCES})
add_executable(wcc_gb src/wcc.c ${SOURCES})
add_executable(example_gb src/example.c ${SOURCES})