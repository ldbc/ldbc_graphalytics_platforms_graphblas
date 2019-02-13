cmake_minimum_required(VERSION 3.13)
project(LDBC_Graphalytics)

set(CMAKE_CXX_STANDARD 17)

# Add GBTL headers to the search path
include_directories(SYSTEM
        src/libs/GBTL/src
        src/libs/GBTL/src/graphblas/platforms/sequential)

# Libraries
add_subdirectory("src/libs/GraphalitycsLoader")

# Products
add_subdirectory("src/GraphBLAS")
#add_subdirectory("GraphBLAS_TL")