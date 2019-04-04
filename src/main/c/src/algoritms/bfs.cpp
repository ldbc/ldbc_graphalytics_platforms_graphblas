/*
 * LCC algorithm implementation in GraphBLAS.
 * Based on SuitSparse::GraphBLAS demo
 */

#include <iostream>
#include <ctime>

extern "C" {
#include <GraphBLAS.h>
}

#include "utils.h"
#include "graphio.h"


GrB_Info bfs6               // BFS of a graph (using unary operator)
        (
                GrB_Vector *v_output,   // v [i] is the BFS level of node i in the graph
                const GrB_Matrix A,     // input graph, treated as if boolean in semiring
                GrB_Index s             // starting node of the BFS
        ) {

    //--------------------------------------------------------------------------
    // set up the semiring and initialize the vector v
    //--------------------------------------------------------------------------

    GrB_Index n;                          // # of nodes in the graph
    GrB_Vector q = NULL;                  // nodes visited at each level
    GrB_Vector v = NULL;                  // result vector
    GrB_Monoid Lor = NULL;                // Logical-or monoid
    GrB_Semiring Boolean = NULL;          // Boolean semiring
    GrB_Descriptor desc = NULL;           // Descriptor for vxm
    GrB_UnaryOp apply_level = NULL;       // unary op: z = f(x) = level

    GrB_Matrix_nrows(&n, A);             // n = # of rows of A
    GrB_Vector_new(&v, GrB_INT32, n);    // Vector<int32_t> v(n) = 0
    GrB_Vector_new(&q, GrB_BOOL, n);     // Vector<bool> q(n) = false
    GrB_Vector_setElement (q, true, s);   // q[s] = true, false elsewhere

    // Note the typecast to bool.  Otherwise an error is reported, since the
    // _Generic function selects the wrong function (int32, not boolean).  This
    // is because of default integer promotion of function arguments in C.
    GrB_Monoid_new (&Lor, GrB_LOR, (bool) false);

    // The semiring uses "AND" as the multiply operator, and "OR" as the
    // addititive monoid.
    GrB_Semiring_new(&Boolean, Lor, GrB_LAND);

    GrB_Descriptor_new(&desc);
    GrB_Descriptor_set(desc, GrB_MASK, GrB_SCMP);     // invert the mask
    GrB_Descriptor_set(desc, GrB_OUTP, GrB_REPLACE);  // clear q first

    // create a unary operator
    GrB_UnaryOp_new (&apply_level, bfs_level, GrB_INT32, GrB_BOOL);

    //--------------------------------------------------------------------------
    // BFS traversal and label the nodes
    //--------------------------------------------------------------------------

    GrB_Index nvals = 1;
    for (level = 1; nvals > 0 && level <= n; level++) {
        // v[q] = level, using apply.  This function applies the unary operator
        // to the entries in q, which are the unvisited successors, and then
        // writes their levels to v, thus updating the levels of those nodes in
        // v.  The patterns of v and q are disjoint.
        GrB_apply (v, NULL, GrB_PLUS_INT32, apply_level, q, NULL);

        // q<!v> = q ||.&& A ; finds all the unvisited
        // successors from current q, using !v as the mask
        GrB_vxm(q, v, NULL, Boolean, q, A, desc);

        GrB_Vector_nvals(&nvals, q);
    }

    *v_output = v;         // return result

    GrB_free (&q);
    GrB_free (&Lor);
    GrB_free (&Boolean);
    GrB_free (&desc);
    GrB_free (&apply_level);

    return (GrB_SUCCESS);
}

int main(int argc, char** argv) {
    BenchmarkParameters benchmarkParameters = ParseCommandLineParameters(argc, argv);
    if (benchmarkParameters.directed) {
        lcc_dir(benchmarkParameters);
    } else {
        lcc_undir(benchmarkParameters);
    }
}
