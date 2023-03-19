/*
 * PageRank algorithm implementation in GraphBLAS.
 */

#include <iostream>
#include <ctime>
#include <fstream>
#include <vector>

#include "utils.h"
#include "graphio.h"
#include "computation_timer.hpp"

/*
 * Result serializer function
 */
void SerializePageRankResult(
    GrB_Vector result,
    const std::vector<GrB_Index> &mapping,
    const BenchmarkParameters &parameters
) {
    std::ofstream file{parameters.output_file};
    if (!file.is_open()) {
        std::cerr << "Output file " << parameters.output_file << " does not exists" << std::endl;
        exit(-1);
    }
    file.precision(16);
    file << std::scientific;

    GrB_Info info;
    GrB_Index n = mapping.size();
    GrB_Index nvals;
    OK(GrB_Vector_nvals(&nvals, result));

    double *X = NULL;
    X = (double *) malloc((n + 1) * sizeof(double));
    OK(GrB_Vector_extractTuples_FP64(GrB_NULL, X, &nvals, result));

    for (GrB_Index matrix_index = 0; matrix_index < n; matrix_index++) {
        GrB_Index original_index = mapping[matrix_index];
        file << original_index << " " << X[matrix_index] << std::endl;
    }

    free(X);
}

GrB_Vector LA_PR(GrB_Matrix A, bool directed, double damping_factor, int iteration_num) {
    ComputationTimer timer{"PageRank"};

    GrB_Info info;
    GrB_Vector result = NULL;

    LAGraph_Kind kind = directed ? LAGraph_ADJACENCY_DIRECTED : LAGraph_ADJACENCY_UNDIRECTED;
    LAGraph_Graph G;

    int niters;
    char msg[LAGRAPH_MSG_LEN];
    LAGraph_New(&G, &A, kind, msg);
    LAGraph_Cached_OutDegree(G, msg);
    LAGraph_Cached_AT(G, msg);
    if (LAGr_PageRankGX(&result, &niters, G, damping_factor, iteration_num, msg) != GrB_SUCCESS) {
        printf("msg: %s\n", msg);
    }

    return result;
}

int main(int argc, char **argv) {
    BenchmarkParameters parameters = ParseBenchmarkParameters(argc, argv);

    LAGraph_Init(NULL);
    GxB_Global_Option_set(GxB_GLOBAL_NTHREADS, parameters.thread_num);

    GrB_Matrix A = ReadMatrixMarket(parameters);
    std::vector<GrB_Index> mapping = ReadMapping(parameters);

    std::cout << "Processing starts at: " << GetCurrentMilliseconds() << std::endl;
    GrB_Vector result = LA_PR(A, parameters.directed, parameters.damping_factor, parameters.max_iteration);
    std::cout << "Processing ends at: " << GetCurrentMilliseconds() << std::endl;

    SerializePageRankResult(result, mapping, parameters);

    GrB_Matrix_free(&A);
    GrB_Vector_free(&result);
}
