cmake_minimum_required(VERSION 3.13)
project(GraphLoader CXX)

set(CMAKE_CXX_STANDARD 17)
set(SOURCES src/graphalitics_loader.cpp)

add_library(GraphLoader STATIC ${SOURCES})
target_include_directories(GraphLoader PUBLIC include)
