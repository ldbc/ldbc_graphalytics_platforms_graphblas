cmake_minimum_required(VERSION 3.13)
project(GraphBLAS C)

set(CMAKE_C_STANDARD 11)

link_directories(/usr/local/lib64)

set(SOURCES
        src/utils.c
        src/graphio.c)

add_executable(lcc src/algoritms/lcc.c ${SOURCES})
target_include_directories(lcc PRIVATE include)
target_link_libraries(lcc graphblas)