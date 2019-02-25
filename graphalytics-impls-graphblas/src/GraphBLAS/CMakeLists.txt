cmake_minimum_required(VERSION 3.13)
project(GraphBLAS)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)
link_directories(/usr/local/lib64)

set(SOURCES src/utils.c)

add_executable(gb_bfs src/bfs.c src/bfs_source.c ${SOURCES})
target_link_libraries(gb_bfs graphblas GraphalitycsLoader)

add_executable(gb_lcc src/lcc.c ${SOURCES})
target_link_libraries(gb_lcc graphblas GraphalitycsLoader)

add_executable(gb_lpa src/lpa.c ${SOURCES})
add_executable(gb_pagerank src/pagerank.c ${SOURCES})
add_executable(gb_sssp src/sssp.c ${SOURCES})
add_executable(gb_wcc src/wcc.c ${SOURCES})
