cmake_minimum_required(VERSION 3.13)
project(GBTL)

set(CMAKE_CXX_STANDARD 17)

# Add GBTL headers to the search path
include_directories(SYSTEM
        ../libs/gbtl/src
        ../libs/gbtl/src/graphblas/platforms/sequential)
include_directories(include)

add_executable(gbtl_lcc src/lcc.cpp)
target_link_libraries(gbtl_lcc GraphalitycsLoader)
