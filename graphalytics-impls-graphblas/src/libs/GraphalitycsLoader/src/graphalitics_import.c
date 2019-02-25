#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "graphalytics_import.h"

size_t LINE_LENGTH = 1024 * 1024;

GR_IndexType GetMaximum(GR_IndexType currentMax, GR_IndexType newValue) {
    if (newValue < INT64_MAX && newValue > currentMax) {
        return newValue;
    } else {
        return currentMax;
    }
}

GraphalyticsMatrix LoadUnweightedMatrix(const char *path, unsigned long tuple_count) {
    GR_IndexType *column_indices = malloc(sizeof(GR_IndexType) * tuple_count);
    GR_IndexType *row_indices = malloc(sizeof(GR_IndexType) * tuple_count);
    GR_ValueType *values = malloc(sizeof(GR_ValueType) * tuple_count);

    FILE *file = fopen(path, "r");
    if (file == NULL) {
        printf("File cannot be opened");
        exit(1);
    }

    // The dimension (width & height) of the matrix
    unsigned long matrix_size = 0;
    // Index to mark the current position in result arrays
    size_t tuple_index = 0;

    // The line will be stored here. This pointer should not be moved
    char *line_buffer = malloc(sizeof(char) * LINE_LENGTH);
    // Pointer used to index into line_buffer
    char* line = NULL;
    // Value used to store how many characters sscanf processed
    int read_chars = 0;

    // While we have a line in the file...
    while (getline(&line_buffer, &LINE_LENGTH, file) != -1) {

        // Extract the source index (first column in file)
        GR_IndexType source_index;
        sscanf(line_buffer, "%lu %n", &source_index, &read_chars);

        // Check if index is the highest index so far
        matrix_size = GetMaximum(matrix_size, source_index);

        // Set the line pointer to the first target vertex (second column in file)
        line = line_buffer + read_chars;

        GR_IndexType target_index;
        // While there is a vertex in the string
        while (sscanf(line, "%lu%n", &target_index, &read_chars) > 0) {
            // Advance the line pointer with the chars sscanf processed
            line += read_chars;
            //printf("%lu -> %lu\n", source_index, target_index);

            // Check if index is the highest index so far
            matrix_size = GetMaximum(matrix_size, target_index);

            // Fill out a line of tuples
            row_indices[tuple_index] = (source_index-1); // Indices in Graphalytics are 1-based
            column_indices[tuple_index] = (target_index-1); // Indices in Graphalytics are 1-based
            values[tuple_index] = 1.0; // Simple directed graphs have uniform weight
            // Advance result array index
            tuple_index++;
        }
    }
    free(line_buffer);

    GraphalyticsMatrix matrix = {
            matrix_size,
            tuple_count,
            row_indices,
            column_indices,
            values
    };

    fclose(file);

    return matrix;
}

GraphalyticsMatrix LoadWeightedMatrix(const char *path) {
    exit(-1);
}