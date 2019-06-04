#pragma once

#include <vector>
#include <iostream>

#include "utils.h"

GrB_Matrix ReadMatrixMarket(BenchmarkParameters parameters);

std::vector<GrB_Index> ReadMapping(BenchmarkParameters parameters);

template <typename T>
void WriteOutResult(
    GrB_Vector result,
    const std::vector<GrB_Index> &mapping,
    const BenchmarkParameters &parameters,
    const T missing_value
) {
    std::ofstream file{parameters.output_file};
    if (!file.is_open()) {
        std::cerr << "File " << parameters.output_file << " does not exists" << std::endl;
        exit(-1);
    }

    double value;
    for (GrB_Index res_index = 0; res_index < mapping.size(); res_index++) {
        GrB_Index original_index = mapping[res_index];
        GrB_Index matrix_index = res_index;

        GrB_Info info = GrB_Vector_extractElement_FP64(&value, result, matrix_index);

        if (info == GrB_SUCCESS) {
            file << original_index << " " << std::scientific << value << std::endl;
        } else {
            file << original_index << " " << missing_value << std::endl;
        }
    }
}