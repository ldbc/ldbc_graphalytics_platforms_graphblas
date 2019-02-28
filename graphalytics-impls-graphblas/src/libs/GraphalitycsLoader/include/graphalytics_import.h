#pragma once

#include <stddef.h>

#include "graphalitics_common.h"

typedef struct {
    // The dimensions of the matrix
    const unsigned long size;
    // The number of tuples loaded
    const GR_IndexType tuple_count;
    // Array with the row (i) indices
    GR_IndexType *rowIndices;
    // Array with the column (j) indices
    GR_IndexType *columnIndices;
    // Array with the values (v)
    GR_ValueType *values;
} GraphalyticsMatrix;

/**
 * Loads an unweighted matrix from the Graphalytics validation set (dir_* or undir_* file).
 * @param path the file containing the structure
 * @return a LoadedMatrix instance containing the loaded file
 */
GraphalyticsMatrix LoadUnweightedMatrix(const char *path, size_t tuple_count);


/**
 * Loads an weighted matrix from the Graphalytics validation set (files which comes with *.e and *.v extensions)
 * @param path the file containing the structure
 * @return a LoadedMatrix instance containing the loaded file
 */
GraphalyticsMatrix LoadWeightedMatrix(const char *path, size_t tuple_count);