//------------------------------------------------------------------------------
// GraphBLAS/Demo/bfs5m.c: breadth first search (mxv and assign/reduce)
//------------------------------------------------------------------------------

// macro used by OK(...) to free workspace if an error occurs
#define FREE_ALL            \
    GrB_free (&A);          \
    GrB_free(&v);

//#include "demos.h"
#include "bfs_source.h"
#include "printer.h"

int main (int argc, char **argv) {

    GrB_Matrix A = NULL;
    GrB_Vector v = NULL;
    GrB_Index n;
    GrB_Index startVertex = 0;

    // Read matrix and get size.
    read_matrix(&A, stdin, false, false, false, false, true);
    OK(GrB_Matrix_nrows(&n, A));

    printIntMatrix(A);

    // Call BFS with A and startVertex.
    bfs(&v, A, startVertex, false, 0);

    // Print the result.
    printIntVector(v);

    // Free GrB objects.
    GrB_free(&A);
    GrB_free(&v);

    return 0;
}