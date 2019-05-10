cmake_minimum_required(VERSION 3.10)
project(GraphBLAS)

set(CMAKE_CXX_STANDARD 11)

link_directories(/usr/local/lib64)
include_directories(include)

find_package(OpenMP REQUIRED)

set(SOURCES
        src/utils.cpp
        src/graphio.cpp)

link_libraries(OpenMP::OpenMP_CXX graphblas lagraph)

add_executable(lcc src/algorithms/lcc.cpp ${SOURCES})
add_executable(bfs src/algorithms/bfs.cpp ${SOURCES})
add_executable(sssp src/algorithms/sssp.cpp ${SOURCES})