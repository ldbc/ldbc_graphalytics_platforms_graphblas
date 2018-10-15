//------------------------------------------------------------------------------
// GraphBLAS/Demo/bfs5m.c: breadth first search (mxv and assign/reduce)
//------------------------------------------------------------------------------


#include "demos.h"
#include "bfs_source.h"
#include "printer.h"

int main (int argc, char **argv) {

    GrB_Matrix A = NULL;
    GrB_Vector v = NULL;
    GrB_Index n;
    GrB_Index startVertex = 0;

    // Read matrix and get size.
    read_matrix(&A, stdin, false, false, false, false, true);
    GrB_Matrix_nrows(&n, A);

    // Call BFS with A and startVertex.
    bfs(&v, A, startVertex, false, 0);

    // Print the result.
    printVector(v);

    // Free GrB objects.
    GrB_free(&A);
    GrB_free(&v);

    return 0;
}