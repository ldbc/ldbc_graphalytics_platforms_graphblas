/*
 * WCC algorithm implementation in GraphBLAS.
 */

#include <algorithm>

extern "C" {
#include <GraphBLAS.h>
#define LAGRAPH_EXPERIMENTAL_ASK_BEFORE_BENCHMARKING
#include <LAGraph.h>
}

#include "utils.h"
#include "graphio.h"
#include "computation_timer.hpp"

void SerializeWCCResult(
    const GrB_Vector result,
    const std::vector<GrB_Index> &mapping,
    const BenchmarkParameters &parameters
) {
    std::ofstream file{parameters.output_file};
    if (!file.is_open()) {
        std::cerr << "File " << parameters.output_file << " does not exists" << std::endl;
        exit(-1);
    }

    GrB_Info info;
    GrB_Index n = mapping.size();
    GrB_Index nvals;
    OK(GrB_Vector_nvals(&nvals, result))

    uint64_t *X = NULL;
    X = (uint64_t *) LAGraph_malloc(n, sizeof(uint64_t));
    OK(GrB_Vector_extractTuples_UINT64(GrB_NULL, X, &nvals, result));

    for (GrB_Index matrix_index = 0; matrix_index < n; matrix_index++) {
        GrB_Index original_index = mapping[matrix_index];
        file << original_index << " " << X[matrix_index] << std::endl;
    }

    LAGraph_free(X);
}

GrB_Vector WeaklyConnectedComponents(GrB_Matrix A, bool directed) {
    ComputationTimer total_timer{"WeaklyConnectedComponents"};
    GrB_Info info;

    GrB_Index n;
    GrB_Matrix_nrows(&n, A);

    GrB_Vector components = NULL;

    // directed graph are symmetric and need to be sanitized
    bool sanitize = directed;
    LAGraph_cc_boruvka(&components, A, sanitize);

    return components;
}

int main(int argc, char **argv) {
    BenchmarkParameters parameters = ParseBenchmarkParameters(argc, argv);

    LAGraph_init();
    GxB_Global_Option_set(GxB_GLOBAL_NTHREADS, parameters.thread_num);

    GrB_Matrix A = ReadMatrixMarket(parameters);
    std::vector<GrB_Index> mapping = ReadMapping(parameters);

    std::cout << "Processing starts at: " << GetCurrentMilliseconds() << std::endl;
    GrB_Vector result = WeaklyConnectedComponents(A, parameters.directed);
    std::cout << "Processing ends at: " << GetCurrentMilliseconds() << std::endl;

    SerializeWCCResult(result, mapping, parameters);

    GrB_Matrix_free(&A);
    GrB_Vector_free(&result);
}
