#include <iostream>
#include <fstream>
#include <string>
#include <limits>

#include "graphio.h"

GrB_Index InsertIntoReindexMap(IndexMap &mapping, GrB_Index index, GrB_Index &mappingIndex) {
    // Try to insert the index with the remapped index if (mappingIndex + 1)
    // If the value exists, this won't overwrite the value.
    auto mappedIndexEmplacing = mapping.emplace(index, mappingIndex + 1);
    // If value was inserted, mappedIndexEmplacing.second will be true
    // so it will increase the mappingIndex by one.
    mappingIndex += mappedIndexEmplacing.second;

    // Return the remapped index
    return mappedIndexEmplacing.first->second;
}

/*
 * Matrix loader function
 */
IndexMap ReadMatrix(BenchmarkParameters parameters, GrB_Matrix &A, bool weighted) {
    // Element index map
    IndexMap mapping;
    GrB_Index mappingIndex = -1;

    std::string filePath{parameters.inputDir};
    // TODO: Replace this with edge.csv
    //filePath += "/edge.csv";

    std::ifstream file{filePath};
    if (!file.is_open()) {
        std::cerr << "File" << filePath << " does not exists" << std::endl;
        exit(-1);
    }

    GrB_Info info;
    OK(GrB_Matrix_new(&A, GrB_FP64, parameters.numVertices, parameters.numVertices));

    while (!file.eof()) {
        // Read the line
        GrB_Index rowIndex, columnIndex;
        double weight = 1.0;

        file >> rowIndex;
        file >> columnIndex;
        if (weighted) {
            file >> weight;
        }

        /*
         * Remap the indexes
         * std::map.emplace will return a tuple upon insert
         * - the first value will be a pointer to the map pair
         * - the second value will be a boolean marking if an update was made
         */

        GrB_Index mappedRowIndex = InsertIntoReindexMap(mapping, rowIndex, mappingIndex);
        GrB_Index mappedColumnIndex = InsertIntoReindexMap(mapping, columnIndex, mappingIndex);

        if (weighted) {
            std::cout
                << mappedRowIndex
                << " -("
                << weight
                << ")-> "
                << mappedColumnIndex
                << std::endl;
        } else {
            std::cout
                << mappedRowIndex
                << " -> "
                << mappedColumnIndex
                << std::endl;
        }

        if (mapping.size() % 100000 == 0) {
            std::cout << "Processing mapping - " << mapping.size() << std::endl;
        }

        OK(GrB_Matrix_setElement_FP64(A, weight, mappedRowIndex, mappedColumnIndex))
        if (!parameters.directed) {
            OK(GrB_Matrix_setElement_FP64(A, weight, mappedColumnIndex, mappedRowIndex))
        }
    }

    return std::move(mapping);
}