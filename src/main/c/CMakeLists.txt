cmake_minimum_required(VERSION 3.10)
project(GraphBLAS)

set(CMAKE_CXX_STANDARD 17)

link_directories(/usr/local/lib64)
include_directories(include)

set(SOURCES src/utils.cpp src/graphio.cpp)

link_libraries(graphblas lagraph stdc++fs)

add_executable(lcc src/algorithms/lcc.cpp ${SOURCES})
#add_executable(bfs src/algorithms/bfs.cpp ${SOURCES})
#add_executable(sssp src/algorithms/sssp.cpp ${SOURCES})

add_executable(converter src/tools/converter.cpp ${SOURCES})