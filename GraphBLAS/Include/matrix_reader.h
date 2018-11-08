//
// Created by lehel on 10/22/18.
//

#ifndef GRAPHBLAS_MATRIX_READER_H
#define GRAPHBLAS_MATRIX_READER_H
#include "demos.h"

GrB_Info read_Matrix_FP64(GrB_Matrix *S, char* path, unsigned int buffer_size, GrB_Index n_rows, GrB_Index n_cols);
GrB_Info read_Matrix_INT32(GrB_Matrix *S, char* path, unsigned int buffer_size, GrB_Index n_rows, GrB_Index n_cols);

#endif //GRAPHBLAS_MATRIX_READER_H
