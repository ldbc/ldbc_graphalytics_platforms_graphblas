#include "graphalytics_loader.hpp"

#include <sstream>
#include <fstream>
#include <iostream>
#include <stdexcept>

LoadedMatrix LoadUnweightedMatrix(const char *path) {
    std::ifstream file(path);

    // Check if file is opened, except throw an error
    if (!file.is_open()) {
        std::stringstream error_msg_str;
        error_msg_str << "File cannot be opened: " << path;
        throw std::runtime_error(error_msg_str.str());
    }

    // The loader code will fill up this struct
    LoadedMatrix matrix = {};

    // Read the file line by line
    while (!file.eof()) {
        std::string line;
        std::getline(file, line);

        std::stringstream stream(line);

        unsigned int source_vertex;
        stream >> source_vertex;

        // Update the expected max size of the matrix
        matrix.size = source_vertex > matrix.size ? source_vertex : matrix.size;

        while (!stream.eof()) {
            unsigned int target_vertex;
            stream >> target_vertex;

            // Update the expected max size of the matrix
            matrix.size = target_vertex > matrix.size ? target_vertex : matrix.size;

            /*
                Build an (i, j, v) tuple.
                Indexes must be decreased, because Graphalytics validation sets
                are indexed from 1, but GraphBLAS works with 0 based indexes.


                GraphBLAS libraries can be initialized from these tuples.
                To make this tuple representable, BLAS does not use a struct,
                but instead of three parallel arrays.
             */
            matrix.rowIndecies.push_back(source_vertex - 1);
            matrix.columnIndecies.push_back(target_vertex - 1);
            matrix.values.push_back(1);

            std::cout << (source_vertex-1) << " --> " << (target_vertex-1) << std::endl;
        }
    }

    return matrix;
}

LoadedMatrix LoadWeightedMatrix(const char *path) {

}