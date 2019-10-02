cmake_minimum_required(VERSION 3.10)
project(GraphBLAS)

set(CMAKE_CXX_STANDARD 17)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "7.0.0")
    message(FATAL_ERROR "Insufficient gcc version")
  endif()
endif()

link_directories(/usr/local/lib64)
include_directories(include)

link_libraries(graphblas lagraph)
add_library(
        graphblas_utils
        SHARED
        src/utils.cpp src/graphio.cpp
)

link_libraries(graphblas_utils)
add_executable(bfs src/algorithms/bfs.cpp)
add_executable(cdlp src/algorithms/cdlp.cpp)
add_executable(lcc src/algorithms/lcc.cpp)
add_executable(pr src/algorithms/pr.cpp)
add_executable(sssp src/algorithms/sssp.cpp)
add_executable(wcc src/algorithms/wcc.cpp)

add_executable(converter src/tools/converter.cpp ${SOURCES})
