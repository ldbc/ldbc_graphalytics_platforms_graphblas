//
// Created by baprof on 2/7/19.
//

#pragma once

#include "graphalytics_loader.hpp"
#include <graphblas/graphblas.hpp>

typedef GraphBLAS ::Vector<float> VectorT;
typedef GraphBLAS::Matrix<float> MatrixT;

MatrixT FillMatrix(const LoadedMatrix& loaded) {
    MatrixT matrix{loaded.size, loaded.size};
    matrix.build(loaded.rowIndecies, loaded.columnIndecies, loaded.values);
    return matrix;
}