cmake_minimum_required(VERSION 3.10)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
  ${CMAKE_SOURCE_DIR}/cmake_modules
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "7.0.0")
    message(FATAL_ERROR "Insufficient gcc version")
  endif()
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  link_directories(/usr/local/lib)

endif()


find_package(GraphBLAS 7.0.1 REQUIRED MODULE)
find_package(LAGraph 1.0.1 REQUIRED MODULE)

include_directories(${CMAKE_SOURCE_DIR}/include
                    ${GRAPHBLAS_INCLUDE_DIR}
                    ${LAGRAPH_INCLUDE_DIR})

add_library(
        graphblas_utils
        SHARED
        src/utils.cpp src/graphio.cpp
)
link_libraries(graphblas_utils)
target_link_libraries(graphblas_utils ${GRAPHBLAS_LIBRARY} ${LAGRAPH_LIBRARIES})

add_executable(bfs src/algorithms/bfs.cpp)
add_executable(cdlp src/algorithms/cdlp.cpp)
add_executable(lcc src/algorithms/lcc.cpp)
add_executable(pr src/algorithms/pr.cpp)
add_executable(sssp src/algorithms/sssp.cpp)
add_executable(wcc src/algorithms/wcc.cpp)

add_executable(converter src/tools/converter.cpp ${SOURCES})
