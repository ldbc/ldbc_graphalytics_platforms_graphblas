#pragma once

#include <graphblas/graphblas.hpp>

GraphBLAS::Matrix<double, GraphBLAS::DirectedMatrixTag> FillMatrixFromFile(const char *path, size_t tuple_count);
