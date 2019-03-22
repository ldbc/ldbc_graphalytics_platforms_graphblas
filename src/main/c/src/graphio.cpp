#include <iostream>
#include <fstream>
#include <string>

#include "graphio.h"

/*
 * Result serializer function
 */
void WriteOutResult(BenchmarkParameters parameters, IndexMap mapping, GrB_Vector result) {
    GrB_Info info;
    GrB_Index n;

    std::ofstream file{parameters.outputFile};
    if (!file.is_open()) {
        std::cerr << "File" << parameters.outputFile << "does not exists" << std::endl;
        exit(-1);
    }

    double value;
    for (auto mappedIndex : mapping) {
        GrB_Index originalIndex = mappedIndex.first;
        GrB_Index matrixIndex = mappedIndex.second;

        value = 0.0;
        GrB_Vector_extractElement_FP64(&value, result, matrixIndex);

        file << originalIndex << " " << value << std::endl;
        std::cout << originalIndex << " " << value << std::endl;
    }
}

/*
 * Matrix loader function
 */
IndexMap ReadMatrix(BenchmarkParameters parameters, GrB_Matrix& A) {
    // Element index map
    IndexMap mapping;
    long mappingIndex = -1;

    std::string filePath{parameters.inputDir};
    // TODO: Replace this with edge.csv
    filePath += "/edge.csv";

    std::ifstream file{filePath};
    if (!file.is_open()) {
        std::cerr << "File" << filePath << "does not exists" << std::endl;
        exit(-1);
    }

    GrB_Info info;
    OK(GrB_Matrix_new(&A, GrB_FP64, parameters.numVertices, parameters.numVertices));

    while (!file.eof()) {
        // Read the line
        GrB_Index rowIndex, columnIndex;
        file >> rowIndex;
        file >> columnIndex;

        auto mappedRowIndexEmplacing = mapping.emplace(rowIndex, mappingIndex + 1);
        GrB_Index mappedRowIndex = mappedRowIndexEmplacing.first->second;
        mappingIndex += mappedRowIndexEmplacing.second;

        auto mappedColumnIndexEmplacing = mapping.emplace(columnIndex, mappingIndex + 1);
        GrB_Index mappedColumnIndex = mappedColumnIndexEmplacing.first->second;
        mappingIndex += mappedColumnIndexEmplacing.second;

        OK(GrB_Matrix_setElement_FP64(A, 1.0, mappedRowIndex, mappedColumnIndex));
        if (!parameters.directed) {
            OK(GrB_Matrix_setElement_FP64(A, 1.0, mappedColumnIndex, mappedRowIndex));
        }
    }

    return std::move(mapping);
}