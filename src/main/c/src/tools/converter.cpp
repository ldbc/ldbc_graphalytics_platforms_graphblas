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
    market_file << "%%MatrixMarket matrix coordinate real";
    if (parameters.directed) {
        market_file << " general" << std::endl;
    } else {
        market_file << " symmetric" << std::endl;
    }
    market_file
        << mappedIndex << " "
        << mappedIndex << " "
        << edge_count << std::endl;

    std::string line;
    Vertex src, trg;
    std::string weight = "1.0";

    std::cout << "Serialize into MM" << std::endl;
    while (std::getline(edge_file, line)) {
        std::istringstream line_stream{line};
        line_stream >> src;
        line_stream >> trg;

        if (parameters.weighted) {
            line_stream >> weight;
        }

        market_file
            << mapping[src] << " "
            << mapping[trg] << " "
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