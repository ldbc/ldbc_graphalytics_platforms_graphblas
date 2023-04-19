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
        std::cerr << "Output file " << parameters.output_file << " does not exists" << std::endl;
        exit(-1);
    }
    file.precision(16);
    file << std::scientific;

    GrB_Info info;
    GrB_Index n = mapping.size();

    GrB_Index nvals;
    OK(GrB_Vector_nvals(&nvals, result))

    GrB_Index *I = NULL;
    double *X = NULL;

    I = (GrB_Index*) malloc ((nvals + 1) * sizeof (GrB_Index));
    X = (double *) malloc((nvals + 1) * sizeof(double));

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

    free(I);
    free(X);
}

GrB_Vector LA_LCC(GrB_Matrix A, bool directed) {
    GrB_Info info;
    GrB_Vector d;

    ComputationTimer timer{"LCC"};

    LAGraph_Kind kind = directed ? LAGraph_ADJACENCY_DIRECTED : LAGraph_ADJACENCY_UNDIRECTED;
    LAGraph_Graph G;

    char msg[LAGRAPH_MSG_LEN];
    LAGraph_lcc(&d, G, msg);

    return d;
}

int main(int argc, char **argv) {
    BenchmarkParameters parameters = ParseBenchmarkParameters(argc, argv);

    LAGraph_Init(NULL);
    GxB_Global_Option_set(GxB_GLOBAL_NTHREADS, parameters.thread_num);

    GrB_Matrix A = ReadMatrixMarket(parameters);
    std::vector<GrB_Index> mapping = ReadMapping(parameters);

    std::cout << "Processing starts at: " << GetCurrentMilliseconds() << std::endl;
    GrB_Vector result = LA_LCC(A, parameters.directed);
    std::cout << "Processing ends at: " << GetCurrentMilliseconds() << std::endl;

    SerializeLCCResult(result, mapping, parameters);

    GrB_Matrix_free(&A);
    GrB_Vector_free(&result);
}
