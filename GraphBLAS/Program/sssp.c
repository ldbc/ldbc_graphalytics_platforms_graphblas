/*
 * Bellman-Ford algorithm linear algebraic implementation based on the "Graph Algorithms in the
 * Language of Linear Algebra" book. The input graph must not contain any negative-weight cycle.
 */

// macro used by OK(...) to free workspace if an error occurs
#define FREE_ALL            \
    GrB_free (&A);          \
    GrB_free(&dist);        \
    GrB_free(&desc);

#include "demos.h"
#include "printer.h"
#include "matrix_reader.h"

int main (int argc, char **argv) {


    GrB_init(GrB_NONBLOCKING);

    GrB_Index n = 4;
    GrB_Index s = 0;
    GrB_Vector dist = NULL;
    GrB_Matrix A = NULL;
    GrB_Descriptor desc = NULL;

    char* path = "/home/lehel/Downloads/SuiteSparse/GraphBLAS/Demo/Matrix/sssp_small";

    read_Matrix_FP64(&A, path, 256, n, n);

    // We know that that a vertex distance from itself is zero. These initial
    // values must be stored in the adjacency matrix.
    for (unsigned int i = 0; i < n; i++) {
        GrB_Matrix_setElement(A, 0.0, i, i);
    }

    // Initialize the distance vector as double and set the s-th element to zero.
    // We use MIN_PLUS semiring, so the identity values are +infinity
    OK(GrB_Vector_new(&dist, GrB_FP64, n));
    OK(GrB_Vector_setElement(dist, 0, s));

    // Set descriptor to clear output first.
    OK(GrB_Descriptor_new(&desc));
    OK(GrB_Descriptor_set(desc, GrB_OUTP, GrB_REPLACE));

    for (unsigned int i = 1; i < n; i++) {
        // dist = dist min.+ A
        OK(GrB_vxm(dist, NULL, NULL, GxB_MIN_PLUS_FP64, dist, A, desc));
    }

    // Print the result.
    printDoubleVector(dist);

    // Free variables.
    GrB_free(&dist);
    GrB_free(&A);
    GrB_free(&desc);

    GrB_finalize();
}