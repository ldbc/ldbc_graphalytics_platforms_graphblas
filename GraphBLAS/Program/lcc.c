/*
 * LCC algorithm implementation in GraphBLAS.
 */

// macro used by OK(...) to free workspace if an error occurs
#define FREE_ALL

#include "demos.h"
#include "printer.h"

// TODO: Try to clear and resize S instead of reallocating every time.
int main (int argc, char **argv) {

    GrB_init(GrB_NONBLOCKING);

    GrB_Index n;
    GrB_Index neighbors_entries;
    GrB_Vector v = NULL;
    GrB_Vector neighbors = NULL;
    GrB_Vector result = NULL;
    GrB_Matrix A = NULL;
    GrB_Matrix S = NULL;
    GrB_Descriptor desc = NULL;

    // Read the adjacency matrix.
    OK(read_matrix(&A, stdin, false, false, false, false, true));

    // Get the number of vertices, or size of the adjacency matrix.
    OK(GrB_Matrix_nrows(&n, A));

    // Initialize the vectors.
    OK(GrB_Vector_new(&v, GrB_INT32, n));
    OK(GrB_Vector_new(&neighbors, GrB_INT32, n));
    OK(GrB_Vector_new(&result, GrB_FP32, n));

    // Set descriptor to clear output first.
    OK(GrB_Descriptor_new(&desc));
    OK(GrB_Descriptor_set(desc, GrB_OUTP, GrB_REPLACE));

    // Iterate through all vertices.
    for (unsigned int i = 0; i < n; i++) {

        // Get the outgoing neighbors of the current vertex by vector
        // matrix multiplication.
        OK(GrB_Vector_setElement(v, 1, i));
        OK(GrB_vxm(neighbors, NULL, NULL, GxB_LOR_LAND_BOOL, v, A, desc));
        OK(GrB_Vector_setElement(v, 0, i));


        // Get tuples from neighbors. A vertex has at most n - 1 neighbors.
        GrB_Index *indices = malloc((n - 1) * sizeof (GrB_Index));
        int *values = malloc((n - 1) * sizeof (int));

        // Count the set values i neighbors.
        GrB_Vector_nvals(&neighbors_entries, neighbors);

        OK(GrB_Vector_extractTuples_INT32(indices, values, &neighbors_entries, neighbors));

        // Initialize the S (subgraph) matrix.
        OK(GrB_Matrix_new(&S, GrB_INT32, neighbors_entries, neighbors_entries));

        // Select the neighbors submatrix from the adjacency matrix.
        OK(GrB_Matrix_extract(S, NULL, NULL, A, indices, neighbors_entries, indices, neighbors_entries, desc));

        // Count the edges among the neighbors by reducing it with scalar addition.
        int edge_number;
        OK(GrB_Matrix_reduce_INT32(&edge_number, NULL, GxB_PLUS_INT32_MONOID, S, NULL));

        // Count lcc for the current vertex, then save it into the result vector.
        float lcc;
        if ((int) neighbors_entries == 1) {
            lcc = 0.0;
        } else {
            lcc = (float) edge_number / (neighbors_entries * (neighbors_entries - 1));
        }

        GrB_Vector_setElement(result, lcc, i);

        // Free the arrays and S.
        free(indices);
        free(values);
        GrB_free(&S);
    }

    printDoubleVector(result);

    // Free other GraphBLAS objects.
    GrB_free(&v);
    GrB_free(&neighbors);
    GrB_free(&result);
    GrB_free(&A);
    GrB_free(&desc);

    GrB_finalize();
}