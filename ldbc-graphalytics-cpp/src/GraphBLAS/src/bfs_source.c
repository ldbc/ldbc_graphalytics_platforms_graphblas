//
// Created by lehel on 10/15/18.
//

// macro used by OK(...) to free workspace if an error occurs
#define FREE_ALL                  \
    GrB_free (&visited);          \
    GrB_free (&neighbors);        \
    GrB_free (&desc);             \
    GrB_free (&LORMonoid);        \
    GrB_free (&boolSemiring);     \


#include "utils.h"
#include "bfs_source.h"

GrB_Info bfs(GrB_Vector *v_output, GrB_Matrix A, GrB_Index startVertex, bool isWcc, int wccLevel) {

    int info;

    GrB_Index n;
    GrB_Vector visited = NULL;
    GrB_Vector neighbors = NULL;
    GrB_Descriptor desc = NULL;
    GrB_Monoid LORMonoid = NULL;
    GrB_Semiring boolSemiring = NULL;

    // Creates a n×n matrix. n is the dimension of A (A is square matrix).
    OK(GrB_Matrix_nrows(&n, A));

    // Create arrays to build vectors.
    GrB_Index* I = NULL;
    int *fill = NULL;

    OK(GrB_Vector_new(&visited, GrB_INT64, n));
    OK(GrB_Vector_new(&neighbors, GrB_INT64, n));

    I = malloc(n * sizeof (GrB_Index));
    fill = malloc(n * sizeof (int));

    for (uint64_t i = 0; i < n; i++) {
        I[i] = i;
        fill[i] = 0;
    }

    OK(GrB_Vector_build(visited, I, fill, n, GrB_FIRST_INT64));
    OK(GrB_Vector_build(neighbors, I, fill, n, GrB_FIRST_INT64));

    OK(GrB_Vector_setElement(neighbors, 1, startVertex));

    // Create descriptor and set it to invert mask and clear the output before assign
    // the new value to it.
    OK(GrB_Descriptor_new(&desc));
    OK(GrB_Descriptor_set(desc, GrB_MASK, GrB_SCMP));
    OK(GrB_Descriptor_set(desc, GrB_OUTP, GrB_REPLACE));

    // Create a boolean monoid and semiring for matrix vector multiplication.
    OK(GrB_Monoid_new(&LORMonoid, GrB_LOR, (bool) false));
    OK(GrB_Semiring_new (&boolSemiring, LORMonoid, GrB_LAND));

    // If used for wcc, use the previous output as input.
    if (isWcc) {
        OK(GrB_Vector_dup(&visited, *v_output));
    }

    bool successor = true;
    for (int32_t level = 1; successor && level <= n; level++) {

        if (isWcc) {
            OK(GrB_Vector_assign_INT64(visited, neighbors, NULL, wccLevel, GrB_ALL, n, NULL));
        } else {
            OK(GrB_Vector_assign_INT64(visited, neighbors, NULL, level, GrB_ALL, n, NULL));
        }

        // neighbors<!visited> = A ||.&& neighbors ; finds all the unvisited
        // successors from current (neighbors), using (!visited) as the mask
        OK(GrB_mxv(neighbors, visited, NULL, boolSemiring, A, neighbors, desc));

        // if there's any new neighbor, proceed.
        OK(GrB_reduce(&successor, NULL, LORMonoid, neighbors, NULL));
    }

    *v_output = visited;

    GrB_free(&neighbors);
    GrB_free(&desc);
    GrB_free(&LORMonoid);
    GrB_free(&boolSemiring);

    return GrB_SUCCESS;
}

