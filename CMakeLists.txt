cmake_minimum_required(VERSION 3.13)
project(LDBC_Graphalytics)

set(CMAKE_CXX_STANDARD 17)

include_directories(SYSTEM
        Dependencies/GBTL/src
        Dependencies/GBTL/src/graphblas/platforms/sequential)

add_subdirectory("GraphLoader")
add_subdirectory("GraphBLAS")
add_subdirectory("GraphBLAS_TL")