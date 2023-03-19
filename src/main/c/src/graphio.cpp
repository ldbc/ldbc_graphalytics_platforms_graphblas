#include "graphio.h"
#include "computation_timer.hpp"

GrB_Matrix ReadMatrixMarket(const BenchmarkParameters& parameters) {
    ComputationTimer total_timer{"Loading the matrix"};

    GrB_Info info;
    GrB_Matrix A;

    if (parameters.binary) {
        auto grb_file_path = parameters.input_dir + "/graph.grb";
        char* grb_file_path_c = strdup(grb_file_path.c_str());
        FILE *grb_file = fopen(grb_file_path_c, "r");
        binread(&A, grb_file);
        fclose(grb_file);
    } else {
        auto market_file_path = parameters.input_dir + "/graph.mtx";
        FILE *mmfile = fopen(market_file_path.c_str(), "r");
        if (mmfile == nullptr) {
            throw std::runtime_error{"Cannot open Matrix market file: " + market_file_path};
        }
        LAGraph_MMRead(&A, mmfile, NULL);
        fclose(mmfile);
    }

    {
        ComputationTimer timer{"Matrix finalization", total_timer};
        GrB_Matrix_wait(A, GrB_MATERIALIZE);
    }

    return A;
}

std::vector<GrB_Index> ReadMapping(const BenchmarkParameters& parameters) {
    ComputationTimer timer{"Loading the mapping"};

    std::vector<GrB_Index> mapping;

    if (parameters.binary) {
        auto vertex_file_path = parameters.input_dir + "/graph.vtb";        
        FILE *vtbfile = fopen(vertex_file_path.c_str(), "r");
        if (vtbfile == nullptr) {
            throw std::runtime_error{"Cannot open Matrix market file"};
        }
        GrB_Index id;
        while (fread(&id, sizeof(GrB_Index), 1, vtbfile) != 0) {
            mapping.push_back(id);
        }
        fclose(vtbfile);
    } else {
        auto vertex_file_path = parameters.input_dir + "/graph.vtx";
        std::ifstream vtx_file{vertex_file_path};

        for (std::string line; std::getline(vtx_file, line);) {
            mapping.push_back(std::stoul(line));
        }
    }

    return mapping;
}
