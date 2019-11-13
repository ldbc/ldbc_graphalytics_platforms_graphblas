/*
 * LCC algorithm implementation in GraphBLAS.
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
void SerializeLCCResult(
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
    GrB_Index *I = NULL;
    double *X = NULL;

    OK(GrB_Vector_nvals(&nvals, result))
    I = (GrB_Index*) LAGraph_malloc ((nvals + 1), sizeof (GrB_Index));
    X = (double *) LAGraph_malloc((nvals + 1), sizeof(double));

    OK(GrB_Vector_extractTuples_FP64(I, X, &nvals, result));

    GrB_Index curr_nz = 0;
    for (GrB_Index matrix_index = 0; matrix_index < n; matrix_index++) {
        GrB_Index original_index = mapping[matrix_index];

        if (I[curr_nz] == matrix_index) {
            file << original_index << " " << X[curr_nz] << std::endl;
            curr_nz++;
        } else {
            file << original_index << " " << 0.0 << std::endl;
        }
    }

    LAGraph_free(I);
    LAGraph_free(X);
}

GrB_Vector LA_LCC(GrB_Matrix A, bool symmetric) {
    GrB_Info info;
    GrB_Vector d;

    {
        ComputationTimer timer{"LCC"};
        double timing[2];
        OK(LAGraph_lcc(&d, A, symmetric, false, timing))
    }

    return d;
}

int main(int argc, char **argv) {
    BenchmarkParameters parameters = ParseBenchmarkParameters(argc, argv);

    LAGraph_init();
    GxB_Global_Option_set(GxB_GLOBAL_NTHREADS, parameters.thread_num);

    GrB_Matrix A = ReadMatrixMarket(parameters);
    std::vector<GrB_Index> mapping = ReadMapping(parameters);

    std::cout << "Processing starts at: " << GetCurrentMilliseconds() << std::endl;
    GrB_Vector result = LA_LCC(A, !parameters.directed);
    std::cout << "Processing ends at: " << GetCurrentMilliseconds() << std::endl;

    SerializeLCCResult(result, mapping, parameters);

    GrB_Matrix_free(&A);
    GrB_Vector_free(&result);
}
