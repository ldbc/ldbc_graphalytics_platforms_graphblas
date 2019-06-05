#include "graphio.h"
#include "computation_timer.hpp"

GrB_Matrix ReadMatrixMarket(const BenchmarkParameters& parameters) {
    ComputationTimer total_timer{"Matrix-Market loading"};

    GrB_Info info;
    GrB_Matrix A;

    auto market_file_path = parameters.input_dir + "/graph.mtx";
    FILE *mmfile = fopen(market_file_path.c_str(), "r");
    if (mmfile == nullptr) {
        throw std::runtime_error{"Cannot open matrix-market"};
    }
    OK(LAGraph_mmread(&A, mmfile))
    fclose(mmfile);

    {
        ComputationTimer timer{"Matrix finalization"};
        GrB_Index nvals;
        GrB_Matrix_nvals(&nvals, A);
    }

    return A;
}

std::vector<GrB_Index> ReadMapping(const BenchmarkParameters& parameters) {
    ComputationTimer timer{"Mapping loading"};

    auto vertex_file_path = parameters.input_dir + "/graph.vtx";
    std::ifstream vtx_file{vertex_file_path};

    std::vector<GrB_Index> mapping;
    for (std::string line; std::getline(vtx_file, line);) {
        mapping.push_back(std::stoul(line));
    }
    return mapping;
}