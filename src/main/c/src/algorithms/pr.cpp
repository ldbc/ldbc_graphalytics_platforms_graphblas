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
        std::cerr << "File " << parameters.output_file << " does not exists" << std::endl;
        exit(-1);
    }
    file.precision(16);
    file << std::scientific;

    GrB_Info info;
    GrB_Index n = mapping.size();
    GrB_Index nvals;
    OK(GrB_Vector_nvals(&nvals, result));

    double *X = NULL;
    X = (double *) LAGraph_malloc((n + 1), sizeof(double));
    OK(GrB_Vector_extractTuples_FP64(GrB_NULL, X, &nvals, result));

    for (GrB_Index matrix_index = 0; matrix_index < n; matrix_index++) {
        GrB_Index original_index = mapping[matrix_index];
        file << original_index << " " << X[matrix_index] << std::endl;
    }

    LAGraph_free(X);
}

GrB_Vector LA_PR(GrB_Matrix A, double damping_factor, unsigned long iteration_num) {
    GrB_Info info;
    GrB_Vector result = NULL;

    {
        ComputationTimer timer{"PageRank"};
        OK(LAGraph_pagerank2(&result, A, damping_factor, iteration_num))
    }

    return result;
}

int main(int argc, char **argv) {
    BenchmarkParameters parameters = ParseBenchmarkParameters(argc, argv);

    LAGraph_init();
    GxB_Global_Option_set(GxB_GLOBAL_NTHREADS, parameters.thread_num);

    GrB_Matrix A = ReadMatrixMarket(parameters);
    std::vector<GrB_Index> mapping = ReadMapping(parameters);

    std::cout << "Processing starts at: " << GetCurrentMilliseconds() << std::endl;
    GrB_Vector result = LA_PR(A, parameters.damping_factor, parameters.max_iteration);
    std::cout << "Processing ends at: " << GetCurrentMilliseconds() << std::endl;

    SerializePageRankResult(result, mapping, parameters);

    GrB_Matrix_free(&A);
    GrB_Vector_free(&result);
}
