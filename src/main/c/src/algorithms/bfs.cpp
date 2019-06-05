/*
 * LCC algorithm implementation in GraphBLAS.
 */

#include <algorithm>

extern "C" {
#include <GraphBLAS.h>
#include <LAGraph.h>
}

#include "utils.h"
#include "graphio.h"
#include "computation_timer.hpp"

void WriteOutBFSResult(
    GrB_Vector result,
    const std::vector<GrB_Index> &mapping,
    const BenchmarkParameters &parameters
) {
    std::ofstream file{parameters.output_file};
    if (!file.is_open()) {
        std::cerr << "File " << parameters.output_file << " does not exists" << std::endl;
        exit(-1);
    }

    GrB_Index value;
    for (GrB_Index res_index = 0; res_index < mapping.size(); res_index++) {
        GrB_Index original_index = mapping[res_index];
        GrB_Index matrix_index = res_index;

        GrB_Info info = GrB_Vector_extractElement_UINT64(&value, result, matrix_index);
        if (info == GrB_SUCCESS) {
            file << original_index << " " << std::scientific << value << std::endl;
        } else {
            file << original_index << " " << "9223372036854775807" << std::endl;
        }
    }
}

/**
 * Global level variable storing the current BFS depth
 */
int32_t level = 0;

/**
 * This is an unary 'result = F(element)' function
 *
 * @param result a pointer to the result vector element
 * @param element a pointer to the applied vector element
 */
void ApplyLevel(int32_t *result, bool *element) {
    // Note this function does not depend on its input.  It returns the value
    // of the global variable level for all inputs.  It is applied to the
    // vector q via GrB_apply, which only applies the unary operator to entries
    // in the pattern.  Entries not in the pattern remain implicit (zero in
    // this case), and then are not added by the GrB_PLUS_INT32 accum function.
    (*result) = level;
}

GrB_Vector BFS(GrB_Matrix A, GrB_Index source_vertex) {
    GrB_init(GrB_NONBLOCKING);
    GxB_Global_Option_set(GxB_GLOBAL_NTHREADS, 1);

    // Variable required by the OK macro
    unsigned int info;

    // The size of Adj
    unsigned long n;
    OK(GrB_Matrix_nrows(&n, A))
    GxB_Matrix_fprint(A, "A", GxB_SUMMARY, stdout);

    std::cout << "Processing starts at: " << GetCurrentMilliseconds() << std::endl;

    //--------------------------------------------------------------------------
    // set up the semiring and initialize the vector v
    //--------------------------------------------------------------------------

    GrB_Vector q = nullptr;                  // nodes visited at each level
    GrB_Vector v = nullptr;                  // result vector
    GrB_Monoid Lor = nullptr;                // Logical-or monoid
    GrB_Semiring Boolean = nullptr;          // Boolean semiring
    GrB_Descriptor desc = nullptr;           // Descriptor for vxm
    GrB_UnaryOp apply_level = nullptr;       // unary op: z = f(x) = level

    GrB_Matrix_nrows(&n, A);             // n = # of rows of A
    GrB_Vector_new(&v, GrB_INT32, n);    // Vector<int32_t> v(n) = 0
    GrB_Vector_new(&q, GrB_BOOL, n);     // Vector<bool> q(n) = false

    // q[s] = true, false elsewhere
    GrB_Vector_setElement_BOOL(q, true, source_vertex);

    // Note the typecast to bool.  Otherwise an error is reported, since the
    // _Generic function selects the wrong function (int32, not boolean).  This
    // is because of default integer promotion of function arguments in C.
    GrB_Monoid_new_BOOL(&Lor, GrB_LOR, false);

    // The semiring uses "AND" as the multiply operator, and "OR" as the
    // addititive monoid.
    GrB_Semiring_new(&Boolean, Lor, GrB_LAND);

    GrB_Descriptor_new(&desc);
    GrB_Descriptor_set(desc, GrB_MASK, GrB_SCMP);     // invert the mask
    GrB_Descriptor_set(desc, GrB_OUTP, GrB_REPLACE);  // clear q first

    // create a unary operator
    GrB_UnaryOp_new(
        &apply_level,
        (void (*)(void *, const void *)) ApplyLevel,
        GrB_INT32,
        GrB_BOOL
    );

    //--------------------------------------------------------------------------
    // BFS traversal and label the nodes
    //--------------------------------------------------------------------------

    GrB_Index nvals = 1;
    for (level = 0; nvals > 0 && level <= n; level++) {
        std::cout << "Level: " << level << std::endl;

        // v[q] = level, using apply.  This function applies the unary operator
        // to the entries in q, which are the unvisited successors, and then
        // writes their levels to v, thus updating the levels of those nodes in
        // v.  The patterns of v and q are disjoint.
        GrB_Vector_apply(v, nullptr, GrB_MIN_INT32, apply_level, q, nullptr);
        //GxB_Vector_fprint(v, "v", GxB_SUMMARY, stdout);

        // q<!v> = q ||.&& A ; finds all the unvisited
        // successors from current q, using !v as the mask
        GrB_vxm(q, v, nullptr, Boolean, q, A, desc);
        //GxB_Vector_fprint(q, "q", GxB_SUMMARY, stdout);

        GrB_Vector_nvals(&nvals, q);
    }

    std::cout << "Processing ends at: " << GetCurrentMilliseconds() << std::endl;

    GrB_Vector_free(&q);
    GrB_Monoid_free(&Lor);
    GrB_Semiring_free(&Boolean);
    GrB_Descriptor_free(&desc);
    GrB_UnaryOp_free(&apply_level);

    return v;
}

GrB_Vector LABFS(GrB_Matrix A, GrB_Index sourceVertex) {
    ComputationTimer timer{"BFS"};

    GrB_Info info;
    GrB_Vector d;
    OK(LAGraph_bfs_simple(&d, A, sourceVertex))

    return d;
}

int main(int argc, char **argv) {
    BenchmarkParameters parameters = ParseBenchmarkParameters(argc, argv);

    LAGraph_init();
    GxB_Global_Option_set(GxB_GLOBAL_NTHREADS, 1);

    GrB_Matrix A = ReadMatrixMarket(parameters);
    std::vector<GrB_Index> mapping = ReadMapping(parameters);

    GrB_Index sourceVertex = std::distance(mapping.begin(), std::find(
        mapping.begin(),
        mapping.end(),
        parameters.source_vertex
    ));

    std::cout << "Processing starts at: " << GetCurrentMilliseconds() << std::endl;
    GrB_Vector result = BFS(A, sourceVertex);
    std::cout << "Processing ends at: " << GetCurrentMilliseconds() << std::endl;

    WriteOutBFSResult(
        result,
        mapping,
        parameters
    );

    GrB_Matrix_free(&A);
    GrB_Vector_free(&result);
}
