//
// Created by lehel on 10/22/18.
//

#include "matrix_reader.h"
#define FREE_ALL            \
    GrB_free (&A);          \

GrB_Info read_Matrix_FP64(GrB_Matrix *S, char* path, unsigned int buffer_size, GrB_Index n_rows, GrB_Index n_cols) {

    GrB_Matrix A = NULL;
    FILE *fp = fopen(path, "r+");

    GrB_Index *row_indices = malloc(buffer_size * sizeof(GrB_Index));
    GrB_Index *col_indices = malloc(buffer_size * sizeof(GrB_Index));
    double *values = malloc(buffer_size * sizeof(double));

    unsigned int entries = 0;
    while(fscanf(fp, "%u %u %lf", (unsigned int*) &row_indices[entries], (unsigned int*) &col_indices[entries],
                 &values[entries]) == 3) {
        entries = entries + 1;
    }
    fclose(fp);
    // Initialize and build the matrix from tuples.
    OK(GrB_Matrix_new(&A, GrB_FP64, n_rows, n_cols));
    OK(GrB_Matrix_build(A, row_indices, col_indices, values, entries, GrB_FIRST_FP64));

    *S = A;

    // Free arrays.
    free(row_indices);
    free(col_indices);
    free(values);

    return GrB_SUCCESS;
}

GrB_Info read_Matrix_INT32(GrB_Matrix *S, char* path, unsigned int buffer_size, GrB_Index n_rows, GrB_Index n_cols) {

    GrB_Matrix A = NULL;
    FILE *fp = fopen(path, "r+");

    GrB_Index *row_indices = malloc(buffer_size * sizeof(GrB_Index));
    GrB_Index *col_indices = malloc(buffer_size * sizeof(GrB_Index));
    int *values = malloc(buffer_size * sizeof(int));

    unsigned int entries = 0;
    while(fscanf(fp, "%u %u %d", (unsigned int*) &row_indices[entries], (unsigned int*) &col_indices[entries],
                 &values[entries]) == 3) {
        entries = entries + 1;
    }
    fclose(fp);
    // Initialize and build the matrix from tuples.
    OK(GrB_Matrix_new(&A, GrB_INT32, n_rows, n_cols));
    OK(GrB_Matrix_build(A, row_indices, col_indices, values, entries, GrB_FIRST_INT32));

    *S = A;

    // Free arrays.
    free(row_indices);
    free(col_indices);
    free(values);

    return GrB_SUCCESS;
}