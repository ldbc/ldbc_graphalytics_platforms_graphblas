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
    std::ofstream market_file{parameters.market_file};
    if (market_file.fail()) {
        throw std::runtime_error{"Failed to open file " + parameters.market_file};
    }

    // Write header
    std::cout << "Write header" << std::endl;

    market_file << "%%MatrixMarket matrix coordinate ";
    market_file << (parameters.weighted ? "real" : "integer");
    market_file << " ";
    market_file << (parameters.directed ? "general" : "symmetric");
    market_file << std::endl;

    market_file << "%%GraphBLAS ";
    market_file << (parameters.weighted ? "GrB_FP64" : "GrB_BOOL");
    market_file << std::endl;

    market_file
        << (mappedIndex - 1) << " "
        << (mappedIndex - 1) << " "
        << edge_count << std::endl;

    std::string line;
    Vertex src, trg;
    std::string weight = "1";

    std::cout << "Serialize MatrixMarket" << std::endl;
    while (std::getline(edge_file, line)) {
        std::istringstream line_stream{line};
        line_stream >> src;
        line_stream >> trg;

        if (parameters.weighted) {
            line_stream >> weight;
        }

        uint64_t first;
        uint64_t second;

        // According to the MatrixMarket specification, for symmetric, skew-symmetric, and Hermitian matrices
        // "Only entries on or below the main diagonal are provided in the file", i.e. A_ij where i >= j
        // (see Table 2 on P9 in https://math.nist.gov/MatrixMarket/reports/MMformat.ps.gz).
        // Therefore, for the symmetric (undirected) case, we flip the entries if mapping[src] < mapping[trg].
        if (!parameters.directed && mapping[src] < mapping[trg]) {
            first = mapping[trg];
            second = mapping[src];
        } else {
            first = mapping[src];
            second = mapping[trg];
        }

        market_file
            << first << " "
            << second << " "
            << weight << std::endl;
    }

    // Open the mapping file
    std::ofstream mapping_file{parameters.mapping_file};
    if (mapping_file.fail()) {
        throw std::runtime_error{"Failed to open file " + parameters.mapping_file};
    }
    std::cout << "Serialize mapping" << std::endl;
    for (auto pair : mapping) {
        mapping_file << pair.first << std::endl;
    }
}