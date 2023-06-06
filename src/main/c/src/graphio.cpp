#include "graphio.h"
#include "computation_timer.hpp"

extern "C" {
#include <LAGraphX.h>
}

GrB_Matrix ReadMatrixMarket(const BenchmarkParameters& parameters) {
    ComputationTimer total_timer{"Loading the matrix"};

    GrB_Info info;
    GrB_Matrix A;

    if (parameters.binary) {
        auto lagraph_file_path = parameters.input_dir + "/graph.lagraph";
        char* lagraph_file_path_c = strdup(lagraph_file_path.c_str());
        GrB_Matrix* M;
        GrB_Index n;
        char* collection_handle;
        LAGraph_SLoadSet(lagraph_file_path_c, &M, &n, &collection_handle, NULL);
        A = *M;
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
