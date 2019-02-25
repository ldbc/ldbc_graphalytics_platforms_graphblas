//------------------------------------------------------------------------------
// GraphBLAS/Demo/src/tri_demo.c: count triangles
//------------------------------------------------------------------------------

// macro used by OK(...) to free workspace if an error occurs
#define FREE_ALL             \
    GrB_free (&v);           \
    GrB_free (&A);           \
    GrB_free (&B);           \
    GrB_free (&desc);

#define PATH "/home/lehel/Downloads/SuiteSparse/GraphBLAS/Demo/Matrix/wcc"

#include "utils.h"
#include "printer.h"
#include "bfs_source.h"
#include "matrix_reader.h"

int main (int argc, char **argv)
{
    GrB_Index n = 10;
    GrB_Index startVertex = 0;
    GrB_Vector v = NULL;
    GrB_Matrix A = NULL;
    GrB_Matrix B = NULL;
    GrB_Descriptor desc = NULL;


    // Read the adjacency matrix.
    read_Matrix_INT32(&A, PATH, 256, n, n);
    //read_matrix(&A, stdin, false, false, false, false, true);
    //OK(GrB_Matrix_nrows(&n, A));

    // Create B matrix as bool matrix.
    OK(GrB_Matrix_new(&B, GrB_BOOL, n, n));

    // Create descriptor to transpose matrix
    OK(GrB_Descriptor_new(&desc));
    OK(GrB_Descriptor_set(desc, GrB_INP1, GrB_TRAN));

    // Create an undirected graph from the directed one by applying
    // elementwise LOR (logical or) on A and A transposed.
    OK(GrB_eWiseAdd_Matrix_BinaryOp(B, NULL, NULL, GrB_LOR, A, A, desc));


    printf("A: \n");

    printIntMatrix(A);

    printf("B: \n");

    printIntMatrix(B);

    // Initialize v.
    OK(GrB_Vector_new(&v, GrB_INT64, n));

    int v_el;
    int bfsLevel = 1;
    for (unsigned int i = 0; i < n; i++) {
        GrB_Info element_info = GrB_Vector_extractElement(&v_el, v, i);
        if (element_info == GrB_NO_VALUE) {
            bfs(&v, B, i, true, bfsLevel);
            bfsLevel = bfsLevel + 1;
        }
    }

    printIntVector(v);

    GrB_free(&v);
    GrB_free(&A);
    GrB_free(&B);
    GrB_free(&desc);


}