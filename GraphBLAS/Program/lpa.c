//
// Created by lehel on 10/22/18.
//

// macro used by OK(...) to free workspace if an error occurs
#define FREE_ALL
#define PATH "/home/lehel/Downloads/SuiteSparse/GraphBLAS/Demo/Matrix/lpa"

#include "demos.h"
#include "printer.h"
#include "matrix_reader.h"

/* Arrange the N elements of ARRAY in random order.
   Only effective if N is much smaller than RAND_MAX;
   if this may not be the case, use a better random
   number generator. */
void shuffle(int *array, size_t n)
{
    if (n > 1)
    {
        size_t i;
        for (i = 0; i < n - 1; i++)
        {
            size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
            int t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}

int main (int argc, char **argv) {

    GrB_Index n = 4;
    GrB_Matrix A = NULL;
    GrB_Vector v_labels = NULL;
    GrB_Vector v_changed = NULL;
    GrB_Vector v_result = NULL;
    GrB_Vector v_neighbors = NULL;
    GrB_Vector v_current_vertex = NULL;
    GrB_Vector v_current_labels = NULL;
    GrB_Descriptor desc = NULL;

    // Read the adjacency matrix.
    read_Matrix_INT32(&A, PATH, 256, n, n);

    printIntMatrix(A);

    // Initialize vectors.
    OK(GrB_Vector_new(&v_labels, GrB_INT32, n));
    OK(GrB_Vector_new(&v_changed, GrB_INT32, n));
    OK(GrB_Vector_new(&v_result, GrB_INT32, n));
    OK(GrB_Vector_new(&v_neighbors, GrB_INT32, n));
    OK(GrB_Vector_new(&v_current_vertex, GrB_INT32, n));
    OK(GrB_Vector_new(&v_current_labels, GrB_INT32, n));

    // Set descriptor to clear output first.
    OK(GrB_Descriptor_new(&desc));
    OK(GrB_Descriptor_set(desc, GrB_OUTP, GrB_REPLACE));

    // Initialize labels.
    for (int i = 0; i < n; i++) {
        OK(GrB_Vector_setElement(v_labels, i + 1, i));
    }

    bool has_changed = true;

    while(has_changed) {

        // Loop through all vertices.
        for (int i = 0; i < n; i++) {
            // Get the current vertex neighbors.
            OK(GrB_Vector_setElement(v_current_vertex, 1, i));
            OK(GrB_vxm(v_neighbors, NULL, NULL, GxB_LOR_LAND_BOOL, v_current_vertex, A, desc));
            GrB_Vector_setElement(v_current_vertex, 0, i);

            // Mask the labels of the neighbors to get current labels.
            OK(GrB_Vector_apply(v_current_labels, v_neighbors, NULL, GrB_IDENTITY_INT32, v_labels, desc));

            GrB_Index* current_indices = malloc(n * sizeof(GrB_Index));
            int* current_values = malloc(n * sizeof(int));
            GrB_Index current_nval = n;

            // Get the current label as array to do argmax.
            OK(GrB_Vector_extractTuples(current_indices, current_values, &current_nval, v_current_labels));

            // Create an array for the frequencies.
            int* freq_arr = malloc (n * sizeof(int));
            // Fill it with zeros first.
            for (int j = 0; j < n; j++) {
                freq_arr[j] = 0;
            }
            // Then with the frequencies.
            for (int j = 0; j < n; j++) {
                freq_arr[current_values[j]] = freq_arr[current_values[j]] + 1;
            }
            //Print current values
            printf("current values:\n");
            for (int j = 0; j < n; j++) {
                printf("%d\n", current_values[j]);
            }
            printf("freq:\n");
            // Print the frequencies
            for (int j = 0; j < n; j++) {
                printf("%d %d\n", j, freq_arr[j]);
            }

            // Search for the maximum of the labels.

            printIntVector(v_neighbors);
            printIntVector(v_current_labels);

            break;
            // Count the most frequent vertices among neighbors.
        }
        break;
    }

}