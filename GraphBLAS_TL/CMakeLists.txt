cmake_minimum_required(VERSION 3.13)
project(GraphBLAS_TL)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

set(SOURCES src/bfs.cpp)

add_executable(bfs_gbtl ${SOURCES})
target_link_libraries(bfs_gbtl GraphLoader)