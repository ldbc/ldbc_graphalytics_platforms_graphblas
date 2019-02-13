cmake_minimum_required(VERSION 3.13)
project(GraphalitycsLoader CXX)

set(CMAKE_CXX_STANDARD 17)
set(SOURCES
        src/graphalitics_import.c
        )

add_library(GraphalitycsLoader STATIC ${SOURCES})
target_include_directories(GraphalitycsLoader PUBLIC include)
