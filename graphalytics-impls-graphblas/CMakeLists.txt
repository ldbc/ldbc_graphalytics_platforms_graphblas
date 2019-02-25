cmake_minimum_required(VERSION 3.13)
project(LDBC_Graphalytics)

set(CMAKE_CXX_STANDARD 17)

# Libraries
add_subdirectory("src/libs/GraphalitycsLoader")

# Products
add_subdirectory("src/GraphBLAS")
add_subdirectory("src/GBTL")