#include <iostream>
#include <fstream>
#include <stdexcept>

#include <algorithm>
#include <string>
#include <sstream>
#include <vector>
#include <map>

#include "graphio.h"

typedef uint64_t Vertex;
typedef std::map<Vertex, Vertex> VertexMapping;

int main(int argc, char **argv) {
    GrB_Info info;
    GrB_Matrix A;

    LAGraph_Init(NULL);

    ConverterParameters parameters = ParseConverterParameters(argc, argv);

    // load
    auto textual_mapping_file_path = parameters.data_dir + "/graph.vtx";
    auto textual_matrix_file_path = parameters.data_dir + "/graph.mtx";

    std::vector<GrB_Index> mapping;
    std::ifstream vtx_file{textual_mapping_file_path};
    for (std::string line; std::getline(vtx_file, line);) {
        mapping.push_back(std::stoul(line));
    }

    FILE *mmfile = fopen(textual_matrix_file_path.c_str(), "r");
    std::cout << "mmread: " << textual_matrix_file_path << std::endl;
    if (mmfile == nullptr) {
        throw std::runtime_error{"Cannot open Matrix Market file: " + textual_matrix_file_path};
    }
    LAGraph_MMRead(&A, mmfile, NULL);
    GrB_Matrix_wait(A, GrB_MATERIALIZE);
    fclose(mmfile);

    // serialize
    auto binary_mapping_file_path = parameters.data_dir + "/graph.vtb";
    auto binary_matrix_file_path = parameters.data_dir + "/graph.grb";

    std::cout << "Serializing binary mapping file (vtb)" << std::endl;
    FILE *binary_mapping_file = fopen(binary_mapping_file_path.c_str(), "w");
    for (GrB_Index index : mapping) {
        fwrite(&index, sizeof(GrB_Index), 1, binary_mapping_file);
    }
    fclose(binary_mapping_file);

    std::cout << "Serializing binary matrix file (grb)" << std::endl;
    FILE *binary_matrix_file = fopen(binary_matrix_file_path.c_str(), "w");
    binwrite(&A, binary_matrix_file, NULL);
    fclose(binary_matrix_file);
    OK(GrB_Matrix_free(&A))

    OK(GrB_finalize())
}
