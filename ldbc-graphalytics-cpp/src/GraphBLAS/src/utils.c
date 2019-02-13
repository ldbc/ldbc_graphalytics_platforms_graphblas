#include "GraphBLAS.h"
#include "utils.h"

#include "graphalytics_import.h"

GrB_Info FillMatrixFromFile(const char *path, size_t tuple_count, GrB_Matrix *matrix) {
    printf("%s\n", path);

    GraphalyticsMatrix loaded = LoadUnweightedMatrix(path, tuple_count);

    GrB_Info info;
    info = GrB_Matrix_new(matrix, GrB_FP64, loaded.size, loaded.size);
    if (info != GrB_SUCCESS) {
        return info;
    }
    info = GrB_Matrix_build(
            *matrix,
            loaded.rowIndices,
            loaded.columnIndices,
            loaded.values,
            loaded.tuple_count,
            GrB_FIRST_FP64);
    if (info != GrB_SUCCESS) {
        return info;
    }

    return GrB_SUCCESS;
}