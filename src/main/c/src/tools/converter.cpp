#include <iostream>
#include <fstream>
#include <stdexcept>

#include <algorithm>
#include <string>
#include <sstream>
#include <vector>
#include <map>

#include "utils.h"

typedef uint64_t Vertex;
typedef std::map<Vertex, Vertex> VertexMapping;

int main(int argc, char **argv) {
    GrB_Info info;
    LAGraph_init();

    ConverterParameters parameters = ParseConverterParameters(argc, argv);

    std::ifstream vertex_file{parameters.vertex_file};
    if (vertex_file.fail()) {
        throw std::runtime_error{"Failed to open file " + parameters.vertex_file};
    }

    VertexMapping mapping;
    Vertex mappedIndex = 1;
    Vertex vertex;
    // Load and remap all the vertices
    std::cout << "Remapping vertices" << std::endl;
    while (vertex_file >> vertex) {
        mapping.insert({vertex, mappedIndex++});
    }

    // Open the edge (*.e) file
    std::ifstream edge_file{parameters.edge_file};
    if (edge_file.fail()) {
        throw std::runtime_error{"Failed to open file " + parameters.edge_file};
    }

    // Count how many lines (or edges) are in the file
    std::cout << "Counting edges" << std::endl;
    size_t edge_count = std::count(
        std::istreambuf_iterator<char>(edge_file),
        std::istreambuf_iterator<char>(),
        '\n'
    );
    edge_file.clear();
    edge_file.seekg(0, std::ios::beg);

    GrB_Matrix A;

    // Open the Matrix Market file
    FILE *mmfile = fopen(parameters.matrix_file.c_str(), "r");
    if (mmfile == nullptr) {
        throw std::runtime_error{"Cannot open Matrix market file: " + parameters.matrix_file};
    }
    OK(LAGraph_mmread(&A, mmfile))
    fclose(mmfile);


    char *matrix_file_c = strdup(parameters.matrix_file.c_str());
    //char *mapping_file_c = strdup(parameters.mapping_file.c_str());
    std::cout << "Serializing binary matrix file (grb)" << std::endl;
    OK(LAGraph_binwrite(&A, matrix_file_c, NULL))

    // TODO: serialize binary mapping file
    // std::cout << "Serializing binary mapping file (vtb)" << std::endl;
    // FILE *f_mapping = fopen(mapping_file_c, "w");
    // for (auto pair : mapping) {
    //     fwrite (&pair.first, sizeof(uint64_t), 1, f_mapping);
    // }
    // fclose(f_mapping);
    OK(GrB_Matrix_free(&A))

    OK(GrB_finalize())
}
