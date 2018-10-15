//------------------------------------------------------------------------------
// GraphBLAS/Demo/Program/tri_demo.c: count triangles
//------------------------------------------------------------------------------

#include "demos.h"
#include "printer.h"
#include "bfs_source.h"

int main (int argc, char **argv)
{
    GrB_Index n;
    GrB_Index startVertex = 0;
    GrB_Vector v = NULL;
    GrB_Matrix A = NULL;
    GrB_Matrix B = NULL;
    GrB_Descriptor desc = NULL;


    // Read A froms stdin.
    read_matrix(&A, stdin, false, false, false, false, true);
    GrB_Matrix_nrows(&n, A);

    // Create B matrix as bool matrix.
    GrB_Matrix_new (&B, GrB_BOOL, n, n);

    // Create descriptor to transpose matrix
    GrB_Descriptor_new(&desc);
    GrB_Descriptor_set(desc, GrB_INP1, GrB_TRAN);

    // Create an undirected graph from the directed one by applying
    // elementwise LOR (logical or) on A and A transposed.
    GrB_eWiseMult_Matrix_BinaryOp(B, NULL, NULL, GrB_LOR, A, A, desc);


    printf("A: \n");

    printMatrix(A);

    printf("B: \n");

    printMatrix(B);

    int v_el;
    int bfsLevel = 0;
    for (unsigned int i = 0; i < n; i++) {
        GrB_Vector_extractElement(&v_el, v, i);
        if (v_el == 0) {
            bfs(&v, B, i, true, bfsLevel);
            bfsLevel = bfsLevel + 1;
        }
    }

    printVector(v);

    GrB_free(&v);
    GrB_free(&A);
    GrB_free(&B);
    GrB_free(&desc);


}