#include "graphio.h"

/*
 * Result serializer function
 */
void WriteOutResult(BenchmarkParameters parameters, GrB_Vector existenceIndicator, GrB_Vector result) {
    GrB_Info info;
    GrB_Index n;

    FILE *file = fopen(parameters.outputFile, "w");
    if (file == NULL) {
        fprintf(stderr, "File %s does not exists\n", parameters.outputFile);
        exit(-2);
    }

    OK(GrB_Vector_size(&n, result));
    double value;
    for (GrB_Index i = 0; i < n; i++) {
        info = GrB_Vector_extractElement(&value, existenceIndicator, i);
        if (info == GrB_SUCCESS) {
            if (GrB_Vector_extractElement(&value, result, i) == GrB_NO_VALUE) {
                value = 0.0;
            }
            fprintf(file, "%lu %.15e\n", (i+1), value);
        }
    }
    fclose(file);
}

/*
 * Matrix loader function
 */
void ReadMatrix(BenchmarkParameters parameters, GrB_Matrix *A) {
    // Determining the array size
    // Undirected matrices will be symmetrized, thus the 2x multiplier
    GrB_Index arraySize = parameters.directed ? parameters.numEdges : parameters.numEdges * 2;

    // Arrays containing tuples
    GrB_Index *rowIndices = malloc(sizeof(GrB_Index) * arraySize);
    GrB_Index *columnIndices = malloc(sizeof(GrB_Index) * arraySize);
    double *values = malloc(sizeof(double) * arraySize);

    // Open the edge-list file
    char inputFilePath[1024];
    snprintf(inputFilePath, 1024, "%s/edge.csv", parameters.inputDir);
    printf("Loading file %s\n", inputFilePath);
    FILE *file = fopen(inputFilePath, "r");
    if (file == NULL) {
        fprintf(stderr, "File %s does not exists", inputFilePath);
        exit(-2);
    }

    // Max index used, which will be used to determine
    // the size of the matrix
    GrB_Index matrixSize = 0;
    // Indexer to the tuple arrays
    GrB_Index bufferIndex = 0;

    while (!feof(file)) {
        // Read the line
        GrB_Index rowIndex, columnIndex;
        fscanf(file, "%lu %lu\n", &rowIndex, &columnIndex);

        // Max check
        matrixSize = matrixSize < rowIndex ? rowIndex : matrixSize;
        matrixSize = matrixSize < columnIndex ? columnIndex : matrixSize;

        // Add the read values to the tuple arrays
        rowIndices[bufferIndex] = rowIndex - 1;
        columnIndices[bufferIndex] = columnIndex - 1;
        values[bufferIndex] = 1.0;
        bufferIndex++;

        // If the matrix is directed, for (u,v) edge
        // create the (v,u) edge too
        if (!parameters.directed) {
            rowIndices[bufferIndex] = columnIndex - 1;
            columnIndices[bufferIndex] = rowIndex - 1;
            values[bufferIndex] = 1.0;
            bufferIndex++;
        }
    }
    fclose(file);

    GrB_Info info;
    info = GrB_Matrix_new(A, GrB_INT32, matrixSize, matrixSize);
    if (info != GrB_SUCCESS) {
        exit(-1);
    }
    info = GrB_Matrix_build(
            *A,
            rowIndices,
            columnIndices,
            values,
            arraySize,
            GrB_FIRST_INT32);
    if (info != GrB_SUCCESS) {
        fprintf(stderr, "GraphBLAS error:\n%s\n", GrB_error());
        exit(-1);
    }
}