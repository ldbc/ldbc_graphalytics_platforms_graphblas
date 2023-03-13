#include "graphio.h"
#include "computation_timer.hpp"

GrB_Matrix ReadMatrixMarket(const BenchmarkParameters& parameters) {
    ComputationTimer total_timer{"Loading the matrix"};

    GrB_Info info;
    GrB_Matrix A;

    auto market_file_path = parameters.input_dir + "/graph.mtx";
    FILE *mmfile = fopen(market_file_path.c_str(), "r");
    if (mmfile == nullptr) {
        throw std::runtime_error{"Cannot open Matrix market file: " + market_file_path};
    }
    LAGraph_MMRead(&A, mmfile, NULL);
    fclose(mmfile);

    {
        ComputationTimer timer{"Matrix finalization", total_timer};
        GrB_Matrix_wait(A, GrB_MATERIALIZE);
    }

    return A;
}

std::vector<GrB_Index> ReadMapping(const BenchmarkParameters& parameters) {
    ComputationTimer timer{"Loading the mapping"};

    std::vector<GrB_Index> mapping;

    auto vertex_file_path = parameters.input_dir + "/graph.vtx";
    std::ifstream vtx_file{vertex_file_path};

    for (std::string line; std::getline(vtx_file, line);) {
        mapping.push_back(std::stoul(line));
    }

    return mapping;
}
