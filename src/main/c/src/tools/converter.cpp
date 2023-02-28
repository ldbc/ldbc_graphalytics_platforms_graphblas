#include <iostream>
#include <fstream>
#include <stdexcept>

#include <algorithm>
#include <string>
#include <sstream>
#include <vector>
#include <map>

#include "utils.h"
#include "graphio.h"

typedef uint64_t Vertex;
typedef std::map<Vertex, Vertex> VertexMapping;

int main(int argc, char **argv) {
    GrB_Info info;
    LAGraph_Init(NULL);

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

    // Open the market file
    std::ofstream matrix_file{parameters.matrix_file};
    if (matrix_file.fail()) {
        throw std::runtime_error{"Failed to open file " + parameters.matrix_file};
    }

    Vertex n = mappedIndex - 1;

    std::cout << "Creating adjacency matrix from edges" << std::endl;
    GrB_Matrix A;
    OK(GrB_Matrix_new(&A, parameters.weighted ? GrB_FP64 : GrB_BOOL, n, n))

    Vertex src, trg;
    std::string line;
    std::string weight_string;

    uint64_t i;
    while (std::getline(edge_file, line)) {
        i++;
        std::istringstream line_stream{line};
        line_stream >> src;
        line_stream >> trg;
        // use 0-based indexing
        Vertex src_mapped = mapping[src] - 1;
        Vertex trg_mapped = mapping[trg] - 1;

        if (parameters.weighted) {
            line_stream >> weight_string;
            OK(GrB_Matrix_setElement_FP64(A, std::stod(weight_string), src_mapped, trg_mapped))
        } else {
            OK(GrB_Matrix_setElement_BOOL(A, 1, src_mapped, trg_mapped))
        }
        if (i % 100000000 == 0) {
            std::cout << "- Edge no." << i << " processed" << std::endl;
        }
    }
    if (!parameters.directed) {
        std::cout << "Converting matrix into symmetric" << std::endl;
        OK(GrB_transpose(A, GrB_NULL, parameters.weighted ? GrB_PLUS_FP64 : GrB_PLUS_BOOL, A, GrB_NULL))
    }

    char *matrix_file_path_c = strdup(parameters.matrix_file.c_str());
    char *mapping_file_path_c = strdup(parameters.mapping_file.c_str());
    if (parameters.binary) {
        std::cout << "Serializing binary matrix file (grb)" << std::endl;
        FILE *matrix_file_c = fopen(matrix_file_path_c, "w");
        binwrite(&A, matrix_file_c, NULL);
        fclose(matrix_file_c);

        std::cout << "Serializing binary mapping file (vtb)" << std::endl;
        FILE *f_mapping = fopen(mapping_file_path_c, "w");
        for (auto pair : mapping) {
            fwrite (&pair.first, sizeof(uint64_t), 1, f_mapping);
        }
        fclose(f_mapping);
    } else {
        std::cout << "Serializing ASCII matrix file (mtx)" << std::endl;
        FILE *f_matrix = fopen(matrix_file_path_c, "w");
        LAGraph_MMWrite(A, f_matrix, NULL, NULL);
        fclose(f_matrix);

        std::cout << "Serializing ASCII mapping file (vtx)" << std::endl;
        std::ofstream mapping_file{parameters.mapping_file};
        if (mapping_file.fail()) {
            throw std::runtime_error{"Failed to open file " + parameters.mapping_file};
        }
        for (auto pair : mapping) {
            mapping_file << pair.first << std::endl;
        }
    }
    OK(GrB_Matrix_free(&A))

    OK(GrB_finalize())
}
