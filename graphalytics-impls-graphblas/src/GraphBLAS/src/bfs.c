//------------------------------------------------------------------------------
// GraphBLAS/Demo/bfs5m.c: breadth first search (mxv and assign/reduce)
//------------------------------------------------------------------------------

// macro used by OK(...) to free workspace if an error occurs
#define FREE_ALL            \
    GrB_free (&A);          \
    GrB_free(&v);

//#include "demos.h"
#include "bfs_source.h"
#include "utils.h"

void WriteOutResult(GrB_Vector result) {
    GrB_Index nvals;
    GrB_Vector_nvals(&nvals, result);

    for (size_t i = 0; i < nvals; i++) {
        GR_IndexType x;
        GrB_Vector_extractElement(&x, result, i);

        x = x == 0 ? INT64_MAX : x-1;
        printf("%lu %lu\n", (i+1), x);
    }
}

int main (int argc, char **argv) {
    GrB_Info info;

    if (argc != 3) {
        printf("Invalid arguments\n");
        exit(-1);
    }

    const char *graph_path = argv[1];
    size_t tuple_count = strtoul(argv[2], NULL, 10);

    GrB_Matrix A = FillMatrixFromFile(graph_path, tuple_count);
    GrB_Vector v = NULL;
    GrB_Index n;
    GrB_Index startVertex = 0;

    // Read matrix and get size.
    OK(GrB_Matrix_nrows(&n, A));

    // Call BFS with A and startVertex.
    bfs(&v, A, startVertex, false, 0);

    //WriteOutResult(v);

    // Free GrB objects.
    //GrB_free(&A);
    GrB_free(&v);

    return 0;
}

