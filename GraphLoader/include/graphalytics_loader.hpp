#pragma once

#include <vector>

typedef struct {
    unsigned int size = 0;
    std::vector<unsigned long> rowIndecies;
    std::vector<unsigned long> columnIndecies;
    std::vector<float> values;
} LoadedMatrix;

/**
 * Loads an unweighted matrix from the Graphalytics validation set (dir_* or undir_* file).
 * @param path the file containing the structure
 * @return a LoadedMatrix instance containing the loaded file
 */
LoadedMatrix LoadUnweightedMatrix(const char* path);

/**
 * Loads an weighted matrix from the Graphalytics validation set (files which comes with *.e and *.v extensions)
 * @param path the file containing the structure
 * @return a LoadedMatrix instance containing the loaded file
 */
LoadedMatrix LoadWeightedMatrix(const char* path);