cmake_minimum_required(VERSION 3.13)
project(GraphBLAS)

set(CMAKE_C_STANDARD 11)

link_directories(/usr/local/lib64)

set(SOURCES
        src/utils.cpp
        src/graphio.cpp)

add_executable(lcc src/algoritms/lcc.cpp ${SOURCES})
target_include_directories(lcc PRIVATE include)
target_link_libraries(lcc graphblas)