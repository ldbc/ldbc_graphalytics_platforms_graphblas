//
// Created by baprof on 6/4/19.
//

#include <fstream>
#include <filesystem>

#include "graphio.h"
#include "computation_timer.hpp"

GrB_Matrix ReadMatrixMarket(BenchmarkParameters parameters) {
    ComputationTimer{"Matrix-Market loading"};

    GrB_Info info;
    GrB_Matrix A;

    std::filesystem::path input_dir{parameters.input_dir};
    FILE *mmfile = fopen(
        (input_dir / "graph.mtx").c_str(),
        "r"
    );
    if (mmfile == nullptr) {
        throw std::runtime_error{"Cannot open matrix-market"};
    }
    OK(LAGraph_mmread(&A, mmfile))
    fclose(mmfile);

    return A;
}

std::vector<GrB_Index> ReadMapping(BenchmarkParameters parameters) {
    ComputationTimer{"Mapping loading"};

    std::filesystem::path input_dir{parameters.input_dir};
    std::ifstream vtx_file{input_dir / "graph.vtx"};

    std::vector<GrB_Index> mapping;
    for (std::string line; std::getline(vtx_file, line);) {
        mapping.push_back(std::stoul(line));
    }
    return mapping;
}
